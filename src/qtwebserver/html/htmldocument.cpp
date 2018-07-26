//
// Copyright 2010-2015 Jacob Dawid <jacob@omg-it.works>
//
// This file is part of QtWebServer.
//
// QtWebServer is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// QtWebServer is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with QtWebServer.
// If not, see <http://www.gnu.org/licenses/>.
//
// It is possible to obtain a commercial license of QtWebServer.
// Please contact Jacob Dawid <jacob@omg-it.works>
//

// Own includes
#include "htmldocument.h"

// Qt includes
#include <QFile>
#include <QStringList>

namespace QtWebServer {

namespace Html {

Document::Document(QString documentTypeDeclaration)
    : QDomDocument() {
    setContent(documentTypeDeclaration);

    _html = createElement(QLatin1String("html"));
    _head = createElement(QLatin1String("head"));
    _body = createElement(QLatin1String("body"));
    QDomElement titleElement = createElement(QLatin1String("title"));
    _title = createTextNode(QLatin1String("Page title"));
    titleElement.appendChild(_title);

    _html.appendChild(_head);
    _html.appendChild(_body);
    _head.appendChild(titleElement);
    appendChild(_html);
}

Document::~Document() {
}

void Document::setTitle(QString title) {
    _title.setNodeValue(title);
}

QString Document::title() {
    return _title.nodeValue();
}

QDomElement Document::html() {
    return _html;
}

QDomElement Document::head() {
    return _head;
}

QDomElement Document::body() {
    return _body;
}

bool Document::appendPartial(QDomElement domElement, QString resourceName) {
    QFile partialFile(resourceName);
    partialFile.open(QFile::ReadOnly);
    if(partialFile.isOpen()) {
        bool success = appendHtml(domElement, QString::fromUtf8(partialFile.readAll()));
        partialFile.close();
        return success;
    }
    return false;
}

bool Document::appendHtml(QDomElement domElement, QString html) {
    QDomDocument domDocument;
    bool parseHtml = domDocument.setContent(html);

    if(!parseHtml) {
        return false;
    }

    // From the Qt documentation on appendChild:
    // Returns a new reference to newChild on success or a null node on failure.
    return !domElement.appendChild(domDocument.documentElement()).isNull();
}

QList<QDomElement> Document::elementsByClass(QString className) const {
    return elementsByAttribute(QLatin1String("class"), className);
}

QList<QDomElement> Document::elementsByClass(QDomElement domElement, QString className) const {
    return elementsByAttribute(domElement, QLatin1String("class"), className);
}

QDomElement Document::elementById(QString idName) const {
    QList<QDomElement> elements = elementsById(idName);
    if(elements.count() > 0) {
        return elements.at(0);
    }
    return QDomElement();
}

QList<QDomElement> Document::elementsById(QString idName) const {
    return elementsByAttribute(QLatin1String("id"), idName);
}

QList<QDomElement> Document::elementsById(QDomElement domElement, QString idName) const {
    return elementsByAttribute(domElement, QLatin1String("id"), idName);
}

QList<QDomElement> Document::elementsByAttribute(QString attributeName,
                                                 QString attributeValue,
                                                 bool allowMultipleValues) const {
    return elementsByAttribute(documentElement(),
                               attributeName,
                               attributeValue,
                               allowMultipleValues);

}

QList<QDomElement> Document::elementsByAttribute(QDomElement domElement,
                                                 QString attributeName,
                                                 QString attributeValue,
                                                 bool allowMultipleValues) const {
    QList<QDomElement> elementList;

    if(domElement.hasAttribute(attributeName)) {
        QString elementAttributeValue = domElement.attribute(attributeName, QLatin1String(""));

        if(allowMultipleValues) {
            QStringList elementAttributeValues = elementAttributeValue.split(QLatin1String("\\s+"), QString::SkipEmptyParts);
            if(elementAttributeValues.contains(attributeValue)) {
                elementList.append(domElement);
            }
        } else {
            if(attributeValue == elementAttributeValue) {
                elementList.append(domElement);
            }
        }
    }

    QDomNodeList children = domElement.childNodes();
    int count = children.count();
    for(int i = 0; i < count; i++) {
        QDomNode child = children.item(i);
        if(child.isElement()) {
            elementList << elementsByAttribute(child.toElement(),
                                               attributeName,
                                               attributeValue,
                                               allowMultipleValues);
        }
    }

    return elementList;
}

} // namespace Html

} // namespace QtWebServer
