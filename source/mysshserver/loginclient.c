#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <assert.h>
#include <pwd.h>


struct pam_conv my_conv = {
    misc_conv,
    NULL,
};

int login_into_user(int sock, char* username)
{
    pam_handle_t *pam;
    int ret;

    ret = pam_start("my_ssh", username, &my_conv, &pam);
    if (ret != PAM_SUCCESS)
    {
        printf("Failed pam_start\n");
        exit(-1);
    }

    int fdstdin = dup(STDIN_FILENO);
    dup2(sock, STDIN_FILENO);

    ret = pam_authenticate(pam, PAM_SILENT);
    if (ret != PAM_SUCCESS)
    {
        printf("Incorrect password!\n");
        exit(-1);
    }

    dup2(fdstdin, STDIN_FILENO);
    close(fdstdin);

    ret = pam_acct_mgmt(pam, 0);
    if (ret != PAM_SUCCESS)
    {
        printf("User account expired!\n");
        exit(-1);
    }

    if (pam_end(pam, ret) != PAM_SUCCESS)
    {
        printf("Unable to pam_end()\n");
        exit(-1);
    }

    printf("login succesfull\n");
    return 0;
}

struct passwd login_client(int sock, struct sockaddr_in* sockinfo, char* username)
{
    assert(sockinfo != NULL);
    assert(username != NULL);

    struct passwd *info;

    info = getpwnam(username);
    if (!info)
    {
        perror("getpwnam");
        exit(-1);
    }

    if (login_into_user(sock, username))
    {
        printf("Unsuccesfull authentification for user %s\n", username);
        exit(-1);
    }

    if (setgid(info->pw_gid))
    {
        perror("setgid");
        exit(-1);
    }

    if (setuid(info->pw_uid))
    {
        perror("setuid");
        exit(-1);
    }

    return *info;
}