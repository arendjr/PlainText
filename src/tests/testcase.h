#ifndef TESTCASE_H
#define TESTCASE_H

#include <QObject>
#include <QScriptValue>


class Engine;

class TestCase : public QObject {

    Q_OBJECT

    protected:
        QScriptValue evaluate(const QString &statement);

    private slots:
        virtual void initTestCase();

        virtual void cleanupTestCase();

    private:
        void createTestWorld();
        void destroyTestWorld();

        Engine *m_engine;
};

#endif // TESTCASE_H
