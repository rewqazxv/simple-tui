#include "meteor-shower.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // atoi, atof

static void help();

typedef struct {
    char *sname;
    char *name;
    char *type;
    void *var;
    char *help;
} par_t;
par_t parv[]= {
    {"-r","--refresh-rate","double",&refresh_rate,"set refresh rate, default is 25Hz"},
    {"-t","--earth-thick","int",&earth_lines,"set how many lines at the bottom as \"earth\", default is 5"},
    {"-e","--earth-char","char",&earth_char,"set character to make up \"earth\", default is '#'"},
    {"-m","--meteor-char","char",&meteor_char,"set character as \"meteor\", default is random uppercase"},
    {"-s","--meteor-speed","double",&demo_speed,"set the slowest \"meteor\"s' speed, default is 7(character/second)"},
    {"-l","--speed-levels","int",&speed_levels,"set how many different speeds, default is 4"},
    {"-n","--meteor-num","int",&n_meteors,"set the quantity of \"meteor\"s, default is 10"},
    {"-N","--no-color","bool",&color_enabled,"disable color output"},
    {"-f","--forever","bool",&endflag,"never end even there is no \"earth\""},
    {"-h","--help","func",(void *)help,"print this help message"},
};
const int parc=sizeof(parv)/sizeof(par_t);

static void help() {
    puts("Available arguments:");
    for (int i=0; i<parc; ++i) {
        printf("  %s, %s",parv[i].sname,parv[i].name);
        // 需要尾随值的情况
        if (strcmp(parv[i].type,"bool") && strcmp(parv[i].type,"func"))
            printf(" <%s>",parv[i].type);
        putchar('\n');
        printf("     %s\n",parv[i].help);
    }
}
static inline void errtip() {
    fputs("Use -h or --help for more infomation.\n",stderr); // 注意fputs不会自动换行
}

#define streq(a,b) (!strcmp((a),(b)))
#define reverse(a) (a)=!(a)
#define testnext \
    if (i+1>=argc) { \
    fprintf(stderr,"Error: missing <%s> after '%s'\n",parv[j].type,argv[i]); \
    errtip(); \
    return 2; \
}

int main(int argc, char *argv[]) {
    for (int i=1; i<argc; ++i) {
        int notfound=1;
        for (int j=0; j<parc; ++j)
            if (streq(argv[i],parv[j].sname) || streq(argv[i],parv[j].name)) {
                notfound=0;
                if (streq(parv[j].type,"int")) {
                    testnext;
                    *(int *)parv[j].var=atoi(argv[++i]);
                } else if (streq(parv[j].type,"double")) {
                    testnext;
                    *(double *)parv[j].var=atof(argv[++i]);
                } else if (streq(parv[j].type,"char")) {
                    testnext;
                    *(char *)parv[j].var=*argv[++i];
                } else if (streq(parv[j].type,"bool"))
                    reverse(*(int *)parv[j].var);
                else if (streq(parv[j].type,"func")) {
                    (*(void (*)())parv[j].var)();
                    return 0;
                } else {
                    fputs("Internal error: unknown argument type\n",stderr);
                    return 4;
                }
                break;
            }
        if (notfound) {
            fprintf(stderr,"Error: unknown option %s\n",argv[i]);
            errtip();
            return 1;
        }
    }

#define caseerr(num,dec) case num:fputs("Error: " dec "\nPlease check the options given.\n",stderr);break;

    int status=start_demo();
    switch (status) {
    case 0:break;
    caseerr(32,"can not show full content");
    caseerr(33,"the slowest meteor's speed must >0");
    caseerr(34,"refresh rate must >0");
    caseerr(37,"earth must have >=0 lines");
    caseerr(35,"meteors must >0");
    caseerr(36,"speed levels must >0");
    default:
        fprintf(stderr,"Undefined error: %d\n",status);
        break;
    }
    return status;
}
