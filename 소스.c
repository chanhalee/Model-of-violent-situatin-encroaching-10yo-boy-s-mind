#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#include"mytype.h"

//#define DEBUG
//#define MAKE_SCRIPT


/*
#define MAX_SIZE 20     // �� Level �� ���� �� �ִ� ����� �ִ�
#define MAX_CHAR 100    // �Էµ� ������ �� ���κ� �ִ��� ��� ������

typedef struct EDGE{
    double weigh;//                                                                                 ���� ����ġ�� �����ϴ� ����
    double dWeigh;//                                                                               ����ġ�� ��ȭ���� �����ϴ� ��
}EDGE;

typedef struct NODE{
    char name[100];
    int activate;
    double criticalPoint;
    EDGE edge[MAX_SIZE];
} NODE;

typedef struct GRAPH{
    int* num;
    NODE* level0;   // ��Ȳ�ν� ����
    NODE* level1;   // ����, �ո� ����
    NODE* level2;   // �����δ� ����
    NODE* level3;   // �ൿ ����
}GRAPH;
*/


//---------------------------------------------------------------------------------

void memFree(GRAPH);
void fileFree(FILE**, int);
NODE* selectLevel(GRAPH*, int);
int askMakeScript(GRAPH);
void makeScript(FILE*, GRAPH, char*);
void recordDefaultWeigh(FILE*, int, int, GRAPH);
void recordCriticalPoint(FILE*, GRAPH, char*);
void recordSimulationInput(FILE*, GRAPH, char*);
void grabAName(FILE*, GRAPH);
void setName(FILE*, GRAPH, int);
void grabAWeigh(FILE*, FILE*, GRAPH);
void grabACritical(FILE*, GRAPH);
void simulate(FILE*, GRAPH);
void readInput(FILE*, GRAPH*);
void inspectInput(GRAPH*);
void transmitSpark(GRAPH*);
void updateWeigh(GRAPH*);
void visualiseCircuit(GRAPH*);
void printSpace(int);
void beginWeighSum(GRAPH*, double*);
void inspectCircuitStatus(GRAPH*, int, int, double*);
void showGraphInfo(GRAPH);
void readGraphInfo(FILE*, int*, int*);

//---------------------------------------------------------------------------------


int main(void) {
    GRAPH graph;
    int l0Node = 0, l1Node = 0;
    int numTemp[5];
    FILE* file[5];
    FILE* basicInfoFp = NULL;
    FILE* initWeighFp = NULL;
    FILE* deltaWeighFp = NULL;
    FILE* criticalPointFp = NULL;
    FILE* simulationInputFp = NULL;

    //                          * read data from files *

    if ((basicInfoFp = fopen("graphInfo.ignite", "r")) == NULL) {
        fprintf(stderr, "\"graphInfo.ignite\"���� ����");
        exit(1);
    }
    file[0] = basicInfoFp;

    readGraphInfo(basicInfoFp, &l0Node, &l1Node);
    numTemp[0] = l0Node;
    numTemp[1] = l1Node;
    numTemp[2] = 5;
    numTemp[3] = 1;
    graph.num = numTemp;

    graph.level0 = (NODE*)malloc(l0Node * sizeof(NODE));
    graph.level1 = (NODE*)malloc(l1Node * sizeof(NODE));
    graph.level2 = (NODE*)malloc(5 * sizeof(NODE));
    graph.level3 = (NODE*)malloc(sizeof(NODE));

    grabAName(basicInfoFp, graph);
#ifdef MAKE_SCRIPT
    askMakeScript(graph);

    memFree(graph);
    fileFree(file, 1);
    return 0;
#endif

    if ((initWeighFp = fopen("presetInfo_Init.ignite", "rt")) == NULL) {
        fprintf(stderr, "\"presetInfo_Init.ignite\"���� ����");
        exit(1);
    }
    file[1] = initWeighFp;
    if ((deltaWeighFp = fopen("presetInfo_Delta.ignite", "r")) == NULL) {
        fprintf(stderr, "\"presetInfo_Delta.ignite\"���� ����");
        exit(1);
    }
    file[2] = deltaWeighFp;
    if ((criticalPointFp = fopen("presetInfo_Critical.ignite", "r")) == NULL) {
        fprintf(stderr, "\"presetInfo_Critical.ignite\"���� ����");
        exit(1);
    }
    file[3] = criticalPointFp;

    grabAWeigh(initWeighFp, deltaWeighFp, graph);//                                                                     ���Ͽ��� �ʱ� ����ġ�� ����ġ ��ȭ���� ���� ������ graph �� �Ű��ִ� �Լ�
    grabACritical(criticalPointFp, graph);  //                                                                          ���Ͽ��� ����� ��� ��ġ���� ���� ������ graph �� �Ű��ִ� ��
#ifdef DEBUG
    showGraphInfo(graph);
#endif


    //                             * run simulation *


    if ((simulationInputFp = fopen("simulation_Input.ignite", "r")) == NULL) {
        fprintf(stderr, "\"presetInfo_Critical.ignite\"���� ����");
        exit(1);
    }
    file[4] = simulationInputFp;

    simulate(simulationInputFp, graph);






    //                            * clean up the mess *

    fileFree(file, 5);
    return 0;
}


//---------------------------------------------------------------------------------



void readGraphInfo(FILE* fp, int* l0, int* l1) {
    char buffer[MAX_CHAR];
    fgets(buffer, MAX_CHAR, fp);
    *l0 = atoi(strtok(buffer, " "));
    *l1 = atoi(strtok(NULL, " "));

    if (*l0 > MAX_SIZE || *l0 < 0 || *l1 > MAX_SIZE || *l1 < 0) {
        printf("/n/n\t\t ERR: ����� ������ MAX_SIZE ���� Ů�ϴ�. graphInfo ������ �����Ͻʽÿ�");
    }

#ifdef DEBUG
    printf("l0 = %d, l1 = %d\n", *l0, *l1);
#endif

}



//-----------------------------------make script manually--------------------------------


int askMakeScript(GRAPH graph) {
    FILE* weighInitFp;
    FILE* weighDeltaFp;
    FILE* criticalPointFp;
    FILE* simulationInputFp;
    int temp1;

    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"presetInfo_Init.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((weighInitFp = fopen("presetInfo_Init.ignite", "w")) == NULL) {
            fprintf(stderr, "��ǻ�� ������� ���� ");
            exit(1);
        }
        makeScript(weighInitFp, graph, "WeighInitInfo");  //                                                �ʱ� ����ġ ���� ����

        fclose(weighInitFp);
    }

    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"presetInfo_Delta.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((weighDeltaFp = fopen("presetInfo_Delta.ignite", "w")) == NULL) {
            fprintf(stderr, "��ǻ�� ������� ���� ");
            exit(1);
        }
        makeScript(weighDeltaFp, graph, "WeighDeltaInfo");  //                                              ����ġ ��ȭ�� ���� ����

        fclose(weighDeltaFp);
    }

    while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"presetInfo_Critical.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((criticalPointFp = fopen("presetInfo_Critical.ignite", "w")) == NULL) {
            fprintf(stderr, "��ǻ�� ������� ���� ");
            exit(1);
        }
        recordCriticalPoint(criticalPointFp, graph, "CriticalPointInfo");  //                               ��� ��ġ ���� ����

        fclose(criticalPointFp);
    }

    while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    printf("\n\n\t\t****WARNING**** \nentering MAKE_SCRIPT mode to write \"simulation_Input.ignite\" (y/n): ");
    temp1 = getc(stdin);
    if (temp1 == 'y') {

        if ((simulationInputFp = fopen("simulation_Input.ignite", "w")) == NULL) {
            fprintf(stderr, "��ǻ�� ������� ���� ");
            exit(1);
        }
        recordSimulationInput(simulationInputFp, graph, "SimulationInput");  //                               ��� ��ġ ���� ����

        fclose(simulationInputFp);
    }
}


void makeScript(FILE* fp, GRAPH graph, char* presetConcat) {
    int level[4];
    char buffer[MAX_CHAR];
    char* token;
    level[0] = graph.num[0];
    level[1] = graph.num[1];
    level[2] = graph.num[2];
    level[3] = graph.num[3];
    //                                      *������ �����Է� �ܰ�*
    printf("preset ������ �Է��Ͻÿ�(���� ���� ����): ");
    scanf("%s", buffer);
    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    token = strcat(buffer, presetConcat);
    fprintf(fp, "\t %s\n", token);

    for (int i = 0; i < 4 - 1; i++) {
        fprintf(fp, "\t\tLv%d -> Lv%d �������� ����\n", i, i + 1);
        printf("\n\t\tLv%d -> Lv%d �������� ����\n", i, i + 1);
        fprintf(fp, "\t");
        for (int k = 0; k < level[i + 1]; k++)
            fprintf(fp, "\tLv%d-n%d", i + 1, k);
        fprintf(fp, "\n");

        for (int j = 0; j < level[i]; j++) {
            fprintf(fp, "\tnode%d:\t", j);
            printf("\n��߳��: %s\n", selectLevel(&graph, i)[j].name);
            for (int l = 0; l < level[i + 1]; l++) {
                recordDefaultWeigh(fp, i, l, graph);
            }
            fprintf(fp, "\n");

        }

    }
}

void recordDefaultWeigh(FILE* fp, int i, int l, GRAPH graph) {
    char buffer[MAX_CHAR];
    char* token;
    double weighTemp;
    printf("������� %s�� ������ ���� �Է�: ", selectLevel(&graph, i + 1)[l].name);
    scanf("%s", buffer);
    token = strtok(buffer, " \n");
    weighTemp = atof(token);
    fprintf(fp, "[%-5.2f]\t", weighTemp);

}

void recordCriticalPoint(FILE* fp, GRAPH graph, char* presetConcat) {
    double criticalTemp;
    char buffer[MAX_CHAR];
    char* token;
    int level[4];
    level[0] = graph.num[0];
    level[1] = graph.num[1];
    level[2] = graph.num[2];
    level[3] = graph.num[3];

    //                                      *������ �����Է� �ܰ�*
    printf("preset ������ �Է��Ͻÿ�(���� ���� ����): ");
    scanf("%s", buffer);
    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    token = strcat(buffer, presetConcat);
    fprintf(fp, "\t %s\n", token);


    for (int i = 0; i < 4; i++) {
        fprintf(fp, "\n\t\tLv%d �� ���� ���\n", i);
        printf("\n\t\tLv%d\n", i);
        for (int j = 0; j < level[i]; j++) {
            fprintf(fp, "\tnode%d:\t", j);
            printf("\nnode%d %s �� ��ġ��: ", j, selectLevel(&graph, i)[j].name);
            scanf("%s", buffer);
            criticalTemp = atof(token);
            fprintf(fp, "[%-5.2f]\t", criticalTemp);

            fprintf(fp, "\n");

        }

    }

}

void recordSimulationInput(FILE* fp, GRAPH graph, char* presetConcat) {
    int proceedTemp = '1', inputTemp, counter = 0;
    char buffer[MAX_CHAR];
    char* token;



    printf("inputFile ������ �Է��Ͻÿ�(���� ���� ����): ");
    scanf("%s", buffer);
    int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
    token = strcat(buffer, presetConcat);
    fprintf(fp, "\t %s\n", token);

    while (proceedTemp == '1') {
        printf("\t\t%d��° simulation input", (counter + 1));
        fprintf(fp, "\n\t\t%d��° simulation input\n", (counter + 1));
        for (int i = 0; i < graph.num[0]; i++) {
            fprintf(fp, "\tnode%d:\t", i);
            printf("\nnode%d %s �� Ȱ��ȭ �Ϸ��� 1�� �Է��ϼ���: ", i, selectLevel(&graph, 0)[i].name);
            inputTemp = getc(stdin);
            while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
            if (inputTemp == '1') {
                fprintf(fp, "[1]\n");
            }
            else {
                fprintf(fp, "[0]\n");
            }

        }
        counter++;
        printf("����Ϸ��� \"1\"�� �Է��ϼ���: ");
        proceedTemp = getchar();
    }




}



//-----------------------------------garb a value from preset--------------------------------



void grabAName(FILE* fp, GRAPH graph) {
    setName(fp, graph, 0);
    setName(fp, graph, 1);
    setName(fp, graph, 2);
    setName(fp, graph, 3);
}

void setName(FILE* fp, GRAPH graph, int lev) {
    char buffer[MAX_CHAR];
    fgets(buffer, MAX_CHAR, fp);    //                                                          �������� ���� �Է����Ͽ� ���ԵǾ��� ���ʿ��� �κ� ����
    for (int i = 0; i < graph.num[lev]; i++) {
        fgets(buffer, MAX_CHAR, fp);
        strtok(buffer, ":\n");  //                                                         ���� ��ū�� �ʿ�����Ƿ� ����
        strcpy(selectLevel(&graph, lev)[i].name, strtok(NULL, ":\n"));

#ifdef DEBUG
        printf("level%d[%d].name = %s\n", lev, i, selectLevel(&graph, lev)[i].name);
#endif
    }
}

void grabAWeigh(FILE* initWeighFp, FILE* deltaWeighFp, GRAPH graph) {
    char buffer1[MAX_CHAR];
    char buffer2[MAX_CHAR];
    char* token;
    double weighTemp;
    NODE* levelPtr;

    fgets(buffer1, MAX_CHAR, initWeighFp);//                                                         ���� ù�ٿ� ��� �������� ����
    fgets(buffer2, MAX_CHAR, deltaWeighFp);

    for (int i = 0; i < 3; i++) {
        levelPtr = selectLevel(&graph, i);
        fgets(buffer1, MAX_CHAR, initWeighFp);//                                                      �� ���ο� level �� ù�ٰ� �ι�° �ٿ� ������ ����
        fgets(buffer1, MAX_CHAR, initWeighFp);
        fgets(buffer2, MAX_CHAR, deltaWeighFp);
        fgets(buffer2, MAX_CHAR, deltaWeighFp);
        for (int j = 0; j < graph.num[i]; j++) {
            fgets(buffer1, MAX_CHAR, initWeighFp);
            fgets(buffer2, MAX_CHAR, deltaWeighFp);

            strtok(buffer1, "[");//                                                                                  ���ο� ��忡 ���� ������ ���� ������ �ռ� ������ �ε��� ����
            for (int k = 0; k < graph.num[i + 1]; k++) {
                token = strtok(NULL, "[]");
                weighTemp = atof(token);
                levelPtr[j].edge[k].weigh = weighTemp;
                strtok(NULL, "[");
            }

            strtok(buffer2, "[");
            for (int k = 0; k < graph.num[i + 1]; k++) {
                token = strtok(NULL, "[]");
                weighTemp = atof(token);
                levelPtr[j].edge[k].dWeigh = weighTemp;
                strtok(NULL, "[");
            }
        }
    }
}

void grabACritical(FILE* fp, GRAPH graph) {
    char buffer[MAX_CHAR];
    char* token;
    double criticalTemp;
    NODE* levelPtr[4];

    fgets(buffer, MAX_CHAR, fp);//                                                                                          ���� ù�ٿ� ��� �������� ����

    for (int i = 0; i < 4; i++) {
        levelPtr[i] = selectLevel(&graph, i);
        fgets(buffer, MAX_CHAR, fp);//                                                                                      ���ο� ������ ���� ������ ���� �ε��� 2�� ����
        fgets(buffer, MAX_CHAR, fp);

        for (int j = 0; j < graph.num[i]; j++) {
            fgets(buffer, MAX_CHAR, fp);
            strtok(buffer, "[");//                                                                                 �� ����� �ε��� ����
            token = strtok(NULL, "[]");
            criticalTemp = atof(token);
            levelPtr[i][j].criticalPoint = criticalTemp;
        }

    }

}




//-----------------------------------------util func--------------------------------------

void memFree(GRAPH graph) {
    free(graph.level0);
    free(graph.level1);
    free(graph.level2);
    free(graph.level3);
}
void fileFree(FILE** file, int num) {
    for (int i = 0; i < num; i++) {
        fclose(file[i]);
    }
}
NODE* selectLevel(GRAPH* graphPtr, int lev) {
    switch (lev) {
    case 0:
        return (*graphPtr).level0;
    case 1:
        return (*graphPtr).level1;
    case 2:
        return (*graphPtr).level2;
    default:
        return (*graphPtr).level3;
    }
}

void showGraphInfo(GRAPH graph) {
    NODE* levelPtr[4];
    printf("\n\t\t********  graphInfo  ********\n");
    for (int i = 0; i < 3; i++) {
        levelPtr[i] = selectLevel(&graph, i);
        printf("\nLv%d node info\n", i);
        for (int j = 0; j < graph.num[i]; j++) {
            printf("Lv%d.node%d\t CP: %5.2lf  ", i, j, levelPtr[i][j].criticalPoint);
            for (int k = 0; k < graph.num[i + 1]; k++) {
                printf("[%5.2lf,%5.2lf] ", levelPtr[i][j].edge[k].weigh, levelPtr[i][j].edge[k].dWeigh);
            }
            printf("\n");
        }
    }
    printf("\nLv%d node info\n", 3);
    printf("Lv%d.node%d\t CP: %5.2lf \n", 3, 0, selectLevel(&graph, 3)[0].criticalPoint);
}


//-------------------------------------visualise--------------------------------

void visualiseCircuit(GRAPH* graph) {
    int superior = 0;
    NODE* levelPtr[4];
    int heightInfo[4];
    int oddEven[4];
    int printCounter[4] = { 0,0,0,0 };

    for (int i = 0; i < 4; i++) {

        levelPtr[i] = selectLevel(graph, i);

        if ((*graph).num[i] % 2 == 0) {//                                                                                 ����� ���ڰ� Ȧ�������� ¦�������� �����ϱ�
            oddEven[i] = 0;
            heightInfo[i] = (*graph).num[i];
        }
        else {
            oddEven[i] = 1;
            heightInfo[i] = (*graph).num[i];
        }

    }

    for (int i = 0; i < 4; i++) {//                                                                                       ���� ū (������ �Ǵ� ���ļ��� ã��)

        if (superior < heightInfo[i]) {
            superior = heightInfo[i];
        }

    }

    for (int counter = 0; counter < superior; counter++) {

        for (int lev = 0; lev < 4; lev++) {

            if ((superior - heightInfo[lev]) / 2 <= counter) {//

                if (printCounter[lev] >= heightInfo[lev]) {
                    printSpace(heightInfo[lev] + 3);
                    printf(" ");
                    printSpace(1);
                    continue;
                }

                if (2 * printCounter[lev] < heightInfo[lev]) {
                    if (printCounter[lev] % 2 == 0) {

                        printSpace((heightInfo[lev] / 2 - printCounter[lev]));
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf("(O)");
                        }
                        else {
                            printf("(X)");
                        }
                        printSpace(printCounter[lev] + (heightInfo[lev] / 2) + 1);

                    }
                    else {

                        printSpace((heightInfo[lev] / 2 + printCounter[lev]));
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf("  (O) ");
                        }
                        else {
                            printf("  (X) ");
                        }
                        printSpace((heightInfo[lev] / 2 - printCounter[lev]));

                    }

                }
                else {
                    if (printCounter[lev] % 2 == 0) {

                        printSpace((-heightInfo[lev] / 2 + printCounter[lev]));
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf("(O)");
                        }
                        else {
                            printf("(X)");
                        }
                        printSpace((heightInfo[lev] * 3) / 2 - printCounter[lev]);

                    }
                    else {

                        printSpace(((heightInfo[lev] * 3) / 2 - printCounter[lev]) - 1);
                        if (levelPtr[lev][printCounter[lev]].activate == 1) {
                            printf(" (O)  ");
                        }
                        else {
                            printf(" (X)  ");
                        }
                        printSpace(printCounter[lev] - heightInfo[lev] / 2);

                    }
                }
                printCounter[lev]++;
                printSpace(2);
                printf(" ");
                printSpace(1);

            }
            else {
                printSpace(heightInfo[lev] + 3);
                printf(" ");
                printSpace(1);

            }
        }
        printf("\n");


    }



}
void printSpace(int num) {
    for (int i = 0; i < num; i++) {
        printf("   ");
    }
}

//------------------------------------run simulation----------------------------

void simulate(FILE* infoFp, GRAPH graph) {
    char buffer[MAX_CHAR];
    char autoTemp = '0';
    int cycleCounter = 0;
    int furyCounter = 0;
    double prevWeighSum[4];
    fgets(buffer, MAX_CHAR, infoFp);//                                                                                        ���� ���� ù���� ���� ���� ��ŵ
    fgets(buffer, MAX_CHAR, infoFp);//                                                                                        ù��° input ���� ������ ������ ����
    beginWeighSum(&graph, prevWeighSum);

    //                          * ��ǲ���� ���� ���� ������ �ùķ��̼� ���� *
    while (!feof(infoFp)) {//                                                                                             ����ȸ�� ���࿩�� ����

        if (cycleCounter != 0 && autoTemp != 'a') {
            printf("\n\ncontinue: y, auto: a, stop: n (y/a/n): ");
            autoTemp = getc(stdin);
            if (autoTemp != 'y' && autoTemp != 'a')
                break;
            int neverUseThis; while (neverUseThis = getchar() != '\n' && neverUseThis != EOF);
        }

        cycleCounter++;
        readInput(infoFp, &graph);

#ifdef DEBUG
        inspectInput(&graph);
#endif

        transmitSpark(&graph);
        updateWeigh(&graph);
        furyCounter += graph.level3[0].activate;

#ifdef DEBUG
        showGraphInfo(graph);
#endif

        printf("\n\t\t* %d��° �ùķ��̼� ��� ��� *\n\n", cycleCounter);
        visualiseCircuit(&graph);
        inspectCircuitStatus(&graph, cycleCounter, furyCounter, prevWeighSum);


        fgets(buffer, MAX_CHAR, infoFp);//                                                                                        �� input ȸ���� ���� �ִ� ������ ����
    }

}

void readInput(FILE* fp, GRAPH* graph) {
    char buffer[MAX_CHAR];
    char* token;
    int inputTemp;

    fgets(buffer, MAX_CHAR, fp);

    for (int i = 0; i < (*graph).num[0]; i++) {

        fgets(buffer, MAX_CHAR, fp);
        strtok(buffer, "[");
        token = strtok(NULL, "[]");
        inputTemp = atoi(token);

        if (inputTemp == 1) {
            (*graph).level0[i].activate = 1;
        }
        else (*graph).level0[i].activate = 0;

    }

}

void inspectInput(GRAPH* graph) {

    printf("\n\n\t�׷��� �Է°� ����! �·ι�ȣ!\n");

    for (int i = 0; i < (*graph).num[0]; i++) {
        printf("\tNODE%d: %d\n", i, (*graph).level0[i].activate);
    }

    printf("\t�̻� ���� ��!\n");
}

void transmitSpark(GRAPH* graph) {
    double sumTemp;
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);

    for (int i = 0; i < 3; i++) {

        for (int j = 0; j < (*graph).num[i + 1]; j++) {

            sumTemp = 0;
            for (int k = 0; k < (*graph).num[i]; k++) {
                sumTemp += (levelPtr[i][k].activate * levelPtr[i][k].edge[j].weigh);
            }

            if (sumTemp > levelPtr[i + 1][j].criticalPoint) {
                levelPtr[i + 1][j].activate = 1;
            }
            else levelPtr[i + 1][j].activate = 0;

        }

    }

}

/*  @@����ġ ������Ʈ ��@@
 * 1. �� ȸ������ ���� ����� ��� ������ ������ �̸� �Էµ� dWeigh ��ŭ ����ġ�� ������Ų��.
 * 2.1 �� ȸ������ ���� ��尡 ���������, ���� ��尡 ������� �ʾ��� ��� ����ġ�� ���ҽ�Ų��.
 * 2.2 �̶� ����ġ�� ���ҽ�Ű�� ��ġ�� ���� ������ ����ġ�� �հ� �����ؼ� ��ü ����ġ�� ���� ������ �ʵ����Ѵ�.(�ε��Ҽ������� ���� ������ ���� �� �ִ�.)
 * 2.2.1 *�߰�* ������ �ټ��� ����ġ ���� ������ �Ҽ��� ���� ������ ������ ��� ���⿧���� �����ϰ� ����Ǵ� ��츦 ����.
 * 2.2.2 *�߰�* ������ ������ ���� ����ġ�� �ް��ϰ� ��ȭ�ϴ� ���� ������, (���Ŀ� ������� �����ϰ� ���� �����ϰ� �� �ʿ� ����)
 * 2.2.3 *�߰�* �������� ������ �����ؼ� ���� ����ġ�� ���� ������ �������� ��� �ǵ��� �ݴ�� ����ġ�� �۵��ϴ� ���� ����
 * 2.3 ��尣�� ����ġ�� ���񰪿� �y���� ����� ����ġ ��ȭ�� ������ �������� ��� �ٸ������ ����ġ ������ ���� ������ ���δ�.
 * 3.1 �����尡 ������� �ʾ����� �����尡 ������� ��� ��� ������ ���⼺�� �Ϲ������� ������ ����ġ�� ��ȭ��Ű�� �ʴ´�.
 * 3.2 ������� �����尡 ��� ������� �ʾ��� ��� ���������� ����ġ�� ��ȭ��Ű�� �ʴ´�.
 */
void updateWeigh(GRAPH* graph) {
    double deltaSum = 0;
    double sumDelta = 0;
    double deltaMul = 0;
    int randomTemp = 0;
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);

    for (int Level_ = 0; Level_ < 2; Level_++) {
        for (int Node_ = 0; Node_ < (*graph).num[Level_]; Node_++) {

            for (int NextNode_ = 0; NextNode_ < (*graph).num[Level_ + 1]; NextNode_++) {

                if ((levelPtr[Level_][Node_].activate == 1) && (levelPtr[Level_ + 1][NextNode_].activate == 1)) {

                    levelPtr[Level_][Node_].edge[NextNode_].weigh += levelPtr[Level_][Node_].edge[NextNode_].dWeigh;
                    deltaSum += fabs(levelPtr[Level_][Node_].edge[NextNode_].dWeigh);//                                       ��Ģ 1.�� �ش��ϴ� ���� ����ġ ��ȭ���� ��
                }
                else {
                    if ((levelPtr[Level_][Node_].activate == 1) && (levelPtr[Level_ + 1][NextNode_].activate != 1)) {//     ��Ģ 2.1�� �ش��ϴ� ������ ����ġ ��ȭ�� ��
                        sumDelta += fabs(levelPtr[Level_][Node_].edge[NextNode_].dWeigh);
                    }


                }

            }

            /*deltaMul = deltaSum / sumDelta;*/ //                                                                      �̷��� �� ��� 2.2.1 ���� �߻�

            deltaMul = (deltaSum + (deltaSum + sumDelta) / (*graph).num[Level_ + 1] * 5) /*                                   2.2.2�� �ӽ� ���� */
                / (sumDelta + (deltaSum + sumDelta) / (*graph).num[Level_ + 1] * 5);

            for (int NextNode_ = 0; NextNode_ < (*graph).num[Level_ + 1]; NextNode_++) {//                                  ��Ģ 2.�� �ش��ϴ� ���� ����ġ ���� ����

                if ((levelPtr[Level_][Node_].activate == 1) && (levelPtr[Level_ + 1][NextNode_].activate != 1)) {
                    levelPtr[Level_][Node_].edge[NextNode_].weigh -= levelPtr[Level_][Node_].edge[NextNode_].dWeigh * deltaMul;
                }

            }

            deltaSum = 0;
            sumDelta = 0;
        }
    }

    /* Level 2�� ����ġ ������� ���� �ٸ���
     * 1. �ൿ�� ��� ���� �ǵ���� �ް�
     * 2. �ൿ���� �ʰ� ��Ȳ�� �Ѿ�� ��� ���� �ǵ���� �޴´�.
     * 3. �ǵ���� rand �Լ��� random ���� �޾� �����Ѵ�.
     */
     //printf("����� random �� ���� ���� ");
    srand((unsigned)clock());
    for (int Level_ = 2; Level_ < 3; Level_++) {
        for (int Node_ = 0; Node_ < (*graph).num[Level_]; Node_++) {
            for (int NextNode_ = 0; NextNode_ < (*graph).num[Level_ + 1]; NextNode_++) {

                if (levelPtr[Level_ + 1][NextNode_].activate == 1) {
                    randomTemp = (rand() % 21) - 10;
                    //printf("%d�� ���: %d  ",Node_, randomTemp);
                    levelPtr[Level_][Node_].edge[NextNode_].weigh += levelPtr[Level_][Node_].edge[NextNode_].dWeigh * randomTemp;
                }
                else {
                    randomTemp = (rand() % 11) - 5;
                    //printf("%d�� ���: %d  ",Node_, randomTemp);
                    levelPtr[Level_][Node_].edge[NextNode_].weigh += levelPtr[Level_][Node_].edge[NextNode_].dWeigh * randomTemp;
                }


            }

        }
    }
}

void beginWeighSum(GRAPH* graph, double* weighSum) {
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);
    for (int level_ = 0; level_ < 3; level_++) {
        for (int node_ = 0; node_ < (*graph).num[level_]; node_++) {
            for (int nextNode_ = 0; nextNode_ < (*graph).num[level_ + 1]; nextNode_++) {
                weighSum[level_] += levelPtr[level_][node_].edge[nextNode_].weigh;
            }
        }
        weighSum[3] += weighSum[level_];
    }
}
void inspectCircuitStatus(GRAPH* graph, int cycleNum, int furyNum, double* prevWeighSum) {
    int nodeNum = 0;
    double weighSum[4] = { 0 };
    int activateNum = 0;
    NODE* levelPtr[4];
    levelPtr[0] = selectLevel(graph, 0);
    levelPtr[1] = selectLevel(graph, 1);
    levelPtr[2] = selectLevel(graph, 2);
    levelPtr[3] = selectLevel(graph, 3);

    for (int level_ = 0; level_ < 4; level_++) {
        nodeNum += (*graph).num[level_];
    }

    for (int level_ = 0; level_ < 3; level_++) {
        for (int node_ = 0; node_ < (*graph).num[level_]; node_++) {
            for (int nextNode_ = 0; nextNode_ < (*graph).num[level_ + 1]; nextNode_++) {
                weighSum[level_] += levelPtr[level_][node_].edge[nextNode_].weigh;
            }
            activateNum += levelPtr[level_][node_].activate;
        }
        weighSum[3] += weighSum[level_];
    }

    printf("\n\n�� ���� Ƚ��: %d\t\t������� ���� ��� ��� Ƚ��: %d\t\t��� �����: %6.2lf\n",
        cycleNum, furyNum, ((double)100 * furyNum) / cycleNum);

    printf("�� ��� ����: %d\t\t�̹� ���� ����� ��� ����: %d\t\t��� �����: %6.2lf\n",
        nodeNum, activateNum, ((double)100 * activateNum) / nodeNum);

    for (int level_ = 0; level_ < 3; level_++) {
        printf("����� Lv%d �� ������ ����ġ ����: %6.2lf\t\t������: %6.2lf\t\t��ȭ �����: %6.2lf\n", level_, weighSum[level_],
            prevWeighSum[level_], ((weighSum[level_] - prevWeighSum[level_]) * 100) / prevWeighSum[level_]);
        prevWeighSum[level_] = weighSum[level_];
    }
    printf("��ü ���� ����ġ ����: %6.2lf\t\t������: %6.2lf\t\t��ȭ �����: %6.2lf\n", weighSum[3],
        prevWeighSum[3], ((weighSum[3] - prevWeighSum[3]) * 100) / prevWeighSum[3]);
    prevWeighSum[3] = weighSum[3];



}
