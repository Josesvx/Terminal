#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#define MAXCOM 1000
#define MAXLIST 100
#define clear() printf("\033[H\033[J")

void inicio_shell()
{
    clear();
    printf("\n\n\n--------Shobe se la come------");
    char *nombreUsuario = getenv("USER");
    printf("\n\n\nusuario: $");
    printf("\n");
    sleep(1);
    clear();
}

int entrada(char *str)
{
    char *buf;
    buf = readline("\n$ ");
    if (strlen(buf) != 0)
    {
        add_history(buf);
        strcpy(str, buf);
        return 0;
    }
    else
    {
        return 1;
    }
}
void imprimrDirectorio()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\nDirect: %s", cwd);
}

void comandos(char **parsed)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        printf("\nError a crear el proceso hijo");
        return;
    }
    else if (pid == 0)
    {
        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nNo se pudo ejercutar el comando");
        }
        exit(0);
    }
    else
    {
        wait(NULL);
        return;
    }
}

void ejectPipe(char **parsed, char **parsedpipe)
{
    int pipefd[2];
    pid_t proc1, proc2;
    if (pipe(pipefd) < 0)
    {
        printf("\nPipe no pudo ser inicializado");
        return;
    }
    proc1 = fork();
    if (proc1 == 0)
    {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        if (execvp(parsed[0], parsed) < 0)
        {
            printf("\nNo se puede ejecutar el primer comando");
            exit(0);
        }
    }
    else
    {
        proc2 = fork();
        if (proc2 < 0)
        {
            printf("\n Error");
            return;
        }
        else
        {
            wait(NULL);
            wait(NULL);
        }
    }
}
void ayuda()
{
    puts("\n---Espacio de ayuda---"
         "\ncomandos soportados:"
         "\nexit: Cerrar la terminal"
         "\ncd: Navegacion de directorio"
         "\nls: Mostrar los archivos de el directorio actual"
         "\n");
    return;
}

int constructorDeComandos(char **parsed)
{
    int NumeroComados = 4;
    int cambioArg = 0;
    char *ListaComandos[NumeroComados];
    char *Usuario;

    ListaComandos[0] = "exit";
    ListaComandos[1] = "cd";
    ListaComandos[2] = "help";
    ListaComandos[3] = "hello";

    for (int i = 0; i < NumeroComados; i++)
    {
        if (strcmp(parsed[0], ListaComandos[i]) == 0)
        {
            cambioArg = i + 1;
            break;
        }
    }
    switch (cambioArg)
    {
    case 1:
        printf("\nAdios\n");
        return 1;
    case 2:
        chdir(parsed[1]);
        return 1;
    case 3:
        ayuda();
        return 1;
    case 4:
        Usuario = getenv("USER");
        printf("\nHola %s. \neste es un prototipo de Sistemas Operativos");
        return 1;
        break;

    default:
        break;
    }
    return 0;
}

int convertirPipe(char *str, char **strpiped)
{
    int i;
    for (i = 0; i < 2; i++)
    {
        strpiped[i] = strsep(&str, "|");
        if (strpiped[i] == NULL)
            break;
    }
    if (strpiped[i] == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void convertirEspacio(char *str, char **parsed)
{
    int i;
    for (i = 0; i < MAXLIST; i++)
    {
        parsed[i] = strsep(&str, " ");
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int ProcesadorString(char *str, char **parsed, char **parsedpipe)
{
    char *strpiped[2];
    int piped = 0;
    piped = convertirPipe(str, strpiped);

    if (piped)
    {
        convertirPipe(strpiped[0], parsed);
        convertirPipe(strpiped[1], parsedpipe);
    }
    else
    {
        convertirPipe(str, parsed);
    }
    if (constructorDeComandos(parsed))
        return 0;
    else
        return 1 + piped;
}
int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char *parsedArgsPiped[MAXLIST];
    int execFlag = 0;
    inicio_shell();

    while (1)
    {
        imprimrDirectorio();
        if (entrada(inputString))
            continue;
        execFlag = ProcesadorString(inputString, parsedArgs, parsedArgsPiped);
        if (execFlag == 1)
            comandos(parsedArgs);

        if (execFlag == 2)
            ejectPipe(parsedArgs, parsedArgsPiped);
    }
    return 0;
}
