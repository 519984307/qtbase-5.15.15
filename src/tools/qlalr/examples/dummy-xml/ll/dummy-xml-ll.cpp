/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QLALR module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
****************************************************************************/

#include <cstdlib>
#include <cstdio>

enum Token {
    EOF_SYMBOL,
    LEFT_ANGLE,
    RIGHT_ANGLE,
    ANY,
};

static int current_char;
static int yytoken;
static bool in_tag = false;

bool parseXmlStream();
bool parseTagOrWord();
bool parseTagName();

inline int nextToken()
{
    current_char = fgetc(stdin);
    if (current_char == EOF) {
        return (yytoken = EOF_SYMBOL);
    } else if (current_char == '<') {
        in_tag = true;
        return (yytoken = LEFT_ANGLE);
    } else if (in_tag && current_char == '>') {
        in_tag = false;
        return (yytoken = RIGHT_ANGLE);
    }
    return (yytoken = ANY);
}

bool parse()
{
    nextToken();
    return parseXmlStream();
}

bool parseXmlStream()
{
    while (parseTagOrWord())
        ;

    return true;
}

bool parseTagOrWord()
{
    if (yytoken == LEFT_ANGLE) {
        nextToken();
        if (! parseTagName())
            return false;
        if (yytoken != RIGHT_ANGLE)
            return false;
        nextToken();

        fprintf (stderr, "*** found a tag\n");

    } else if (yytoken == ANY) {
        nextToken();
    } else {
        return false;
    }
    return true;
}

bool parseTagName()
{
    while (yytoken == ANY)
        nextToken();

    return true;
}

int main()
{
    if (parse())
        printf("OK\n");
    else
        printf("KO\n");
}
