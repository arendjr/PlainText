#ifndef TRIGGERREGISTRY_H
#define TRIGGERREGISTRY_H

#include <QMap>
#include <QObject>
#include <QStringList>


class TriggerRegistry : public QObject {

    Q_OBJECT

    public:
        explicit TriggerRegistry(QObject *parent = 0);
        virtual ~TriggerRegistry();

        Q_INVOKABLE void registerTrigger(const QString &signature, const QString &description);

        const QMap<QString, QString> &triggers();

        Q_INVOKABLE QStringList signatures() const;
        Q_INVOKABLE QString description(const QString &signature) const;

    private:
        QMap<QString, QString> m_triggers;
};

#endif // TRIGGERREGISTRY_H
