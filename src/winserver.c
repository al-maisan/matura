#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#pragma comment(lib, "wsock32.lib")

logeintrag()
{
    time_t t = time(NULL);
    struct tm time = *localtime(&t);
    FILE *zeitupdate;
    zeitupdate = fopen("C:\\temp\\matura.log", "a");
    fprintf(zeitupdate, "neue verb. am: %d-%d-%d, %d:%d:%d,", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
    return 1;
}


int main(int argc, char *argv[])
{
    WSADATA wsa;
    SOCKET mysock, newsock;
    struct sockaddr_in server, client;
    int addrlen;
    int portnr;
    int n;
    char stringbuffer[512];
    char spielername[256];
    const char *greeting = "Willkommen, gib deinen Spielernamen ein: ";

    if (argc < 2)
    {
        printf("Syntax: <program> <port number>");
    }

    portnr = atoi(argv[1]);
    if(portnr < 2000)
    {
        printf("WARNING: Port numbers under 2000 can be reserved and are not recommended\n");
    }


    /* wsastartup arg1 is version we want to load, arg2 is a WSADATA-type
    where stuff gets stored when winsock has been loaded */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        wprintf(L"Fehler beim Initialisieren: %ld\n", WSAGetLastError());
        return 1;
    }

    /*next step: create socket using the socket() funktion */
    if((mysock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        wprintf(L"Fehler beim Anlegen des sockets: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portnr);

    if(bind(mysock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        wprintf(L"Socket Bind failed with error code: %ld\n", WSAGetLastError());
        closesocket(mysock);
        WSACleanup();
        return 1;
    }

    listen(mysock, 3);
    printf("waiting for incoming connections\n");

    addrlen = sizeof(struct sockaddr_in);
    newsock = accept(mysock, NULL, NULL);
    if (newsock == INVALID_SOCKET)
    {
        wprintf(L"Fehler beim annehmen eingehender Verbindung: %ld\n", WSAGetLastError());
        closesocket(mysock);
        WSACleanup();
        return 1;
    }


    logeintrag();

    memset(stringbuffer, 0, sizeof(stringbuffer));
    memset(spielername, 0, sizeof(spielername));
    n = write(newsock, greeting, sizeof(greeting));
    if (n < 0)
    {
        fprintf(stderr, "Fehler beim Senden der Daten, %s\n", strerror(errno));
    }

    n = read(newsock, stringbuffer, 256);
    if (n < 0)
    {
        fprintf(stderr, "Fehler beim Empfangen der Daten, %s\n", strerror(errno));
    }

    n = write(newsock, "Datenaustausch erfolgreich", 27);
    if (n < 0)
    {
        fprintf(stderr, "Fehler beim Senden der Antwort, %s\n", strerror(errno));
    }

    strcpy(spielername, stringbuffer);

    FILE *filepointer;

    filepointer = fopen("C:\\temp\\ausgabe.txt", "w");
    if (filepointer == 0)
    {
        fprintf(stderr, "Fehler beim Öffnen der Datei zum Schreiben , %s\n", strerror(errno));
    }

    fprintf(filepointer, spielername);

    fclose(filepointer);

    closesocket(mysock);
    closesocket(newsock);
    WSACleanup();
    return 0;
}
