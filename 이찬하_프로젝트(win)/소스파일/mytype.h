#pragma once
//
// Created by ������ on 2021/01/13.
//

#ifndef VIP_MYTYPE_H
#define VIP_MYTYPE_H

#define MAX_SIZE 20     // �� Level �� ���� �� �ִ� ����� �ִ�
#define MAX_CHAR 300    // �Էµ� ������ �� ���κ� �ִ��� ��� ������

typedef struct EDGE {
    double weigh;//     ���� ����ġ�� �����ϴ� ����
    double dWeigh;//    ����ġ�� ��ȭ���� �����ϴ� ����
}EDGE;

typedef struct NODE {
    char name[100]; // ����� �̸��� ����
    int activate;   // ����� ��� ���θ� ����
    double criticalPoint;   // ����� ��� ��ġ�� ����
    EDGE edge[MAX_SIZE];    // ��忡 ���� ������ ������ ����
} NODE;

typedef struct GRAPH {
    int* num;
    NODE* level0;   // ��Ȳ�ν� ����
    NODE* level1;   // ����, �ո� ����
    NODE* level2;   // �����δ� ����
    NODE* level3;   // �ൿ ����
}GRAPH;
#endif //VIP_MYTYPE_H
