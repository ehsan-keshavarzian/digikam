/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-09-14
 * Description : a parse results map for token management
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef PARSERESULTS_H
#define PARSERESULTS_H

// Qt includes

#include <QPair>
#include <QMap>
#include <QString>

namespace Digikam
{

class ParseResults
{
public:

    typedef QPair<int, int>                 ResultsKey;
    typedef QPair<QString, QString>         ResultsValue;
    typedef QMap<ResultsKey, ResultsValue>  ResultsMap;
    typedef QMap<int, int>                  ModifierIDs;

public:

    ParseResults()   {};
    ~ParseResults()  {};

    void addEntry(int pos, const QString& token, const QString& result);
    void addModifier(int pos, int length);

    QString result(int pos, int length);
    QString token(int pos, int length);


    ResultsKey keyAtPosition(int pos);
    bool       isKeyAtPosition(int pos);
    ResultsKey keyAtApproximatePosition(int pos);
    bool       isKeyAtApproximatePosition(int pos);

    bool isModifier(int pos);

    bool isEmpty();
    void clear();

    QString replaceTokens(const QString& markedString);

private:

    ResultsKey createInvalidKey();
    bool       keyIsValid(const ResultsKey& key);

private:

    ResultsMap  m_results;
    ModifierIDs m_modifiers;
};

} // namespace Digikam


#endif /* PARSERESULTS_H */
