#include <QObject>


class Engine;

class TestCase : public QObject {

    Q_OBJECT

    private slots:
        virtual void initTestCase();

        virtual void init();

        virtual void cleanup();

        virtual void cleanupTestCase();

    private:
        void createTestWorld();
        void destroyTestWorld();

        Engine *m_engine;
};
