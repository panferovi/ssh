#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <assert.h>
#include <pwd.h>

void msgsend(int sock, struct sockaddr_in *sockinfo, char* data);

struct pam_conv my_conv = {
    misc_conv,
    NULL,
};

int login_into_user(int sock, struct sockaddr_in* sockinfo, char* username)
{
    pam_handle_t *pam;
    pid_t pid = getpid();
    char servicename[256] = "";
    sprintf(servicename, "%s_%d", username, pid);

    int ret = pam_start(servicename, username, &my_conv, &pam);
    if (ret != PAM_SUCCESS)
    {
        msgsend(sock, sockinfo, "Failed pam_start\n");
        exit(-1);
    }

    int fdstdin  = dup(STDIN_FILENO);
    dup2(sock, STDIN_FILENO);

    ret = pam_authenticate(pam, PAM_SILENT);
    if (ret != PAM_SUCCESS)
    {
        msgsend(sock, sockinfo, "Incorrect password!\n");
        exit(-1);
    }

    dup2(fdstdin,  STDIN_FILENO);
    close(fdstdin);

    ret = pam_acct_mgmt(pam, 0);
    if (ret != PAM_SUCCESS)
    {
        msgsend(sock, sockinfo, "User account expired!\n");
        exit(-1);
    }

    if (pam_end(pam, ret) != PAM_SUCCESS)
    {
        msgsend(sock, sockinfo, "Unable to pam_end()\n");
        exit(-1);
    }

    msgsend(sock, sockinfo, "login succesfull\n");
    return 0;
}

struct passwd login_client(int sock, struct sockaddr_in* sockinfo, char* username)
{
    assert(sockinfo != NULL);
    assert(username != NULL);

    struct passwd *info = getpwnam(username);
    if (!info)
    {
        perror("getpwnam");
        exit(-1);
    }

    if (login_into_user(sock, sockinfo, username))
    {
        msgsend(sock, sockinfo, "Unsuccesfull authentification for user\n");
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

    if (chdir(info->pw_dir) == -1)
    {
        perror("chdir");
        exit(-1);
    }

    return *info;
}

void msgsend(int sock, struct sockaddr_in *sockinfo, char* data)
{
    assert (sockinfo != NULL);
    assert (data 	 != NULL);

	if (sendto(sock, data, strlen(data), MSG_CONFIRM, 
					(struct sockaddr*) sockinfo, sizeof(*sockinfo)) == -1)
    {
        perror("send");
        exit(-1);
    }
}
