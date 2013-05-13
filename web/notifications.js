/*global define:false, require:false*/
define(["controller"], function(Controller) {

    "use strict";

    var isActive = true;

    function init() {

        attachListeners();
    }

    function attachListeners() {

        window.addEventListener("focus", function() {
            isActive = true;
        }, false);

        window.addEventListener("blur", function() {
            isActive = false;
        }, false);

        function requestPermission() {
            Notification.requestPermission(function() {
                Controller.removeCommandListener(requestPermission);
            });
        }

        Controller.addCommandListener(requestPermission);

        Controller.addIncomingMessageListener(function(message) {
            if (!isActive && Notification.permission === "granted" &&
                /^\w+ (asks|says|tells|shouts)/.test(message)) {

                var notification = new Notification(document.title, {
                    "body": message,
                    "onshow": function() {
                        setTimeout(function() {
                            notification.close();
                        }, 10000);
                    }
                });

            }
        });
    }

    init();
});
