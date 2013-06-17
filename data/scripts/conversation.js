
/**
 * Constructor.
 *
 * This class provides the Conversation Module of the Personality Engine.
 */
function Conversation(personality) {

    this.name = personality.name;

    this.personality = personality;

    this.partners = [];

    this.bok = {
        persons: {},
        self: {
            day: [
                "It's alright, nothing special."
            ],
            health: [],
            mood: [
                "I'm fine, alright.",
                "I'm good, thanks."
            ],
            work: []
        }
    };
}

Conversation.initSentencePatterns = function(sentences) {

    for (var pattern in sentences) {
        var sentence = sentences[pattern];
        sentence.regex = new RegExp("^" + pattern.replace("%1", "(\w+)") + "$");
    }
    return sentences;
};

Conversation.SENTENCE_PATTERNS = Conversation.initSentencePatterns({
    "how are you": { op: "read", paths: ["self.health", "self.mood"] },
    "how is %1": { op: "read", paths: ["persons.%1.health", "persons.%1.mood"] },
    "how is your %1": { op: "read", paths: ["self.%1"] }
});

/**
 * Evaluates whether a sentence from a given character is addressed to me.
 *
 * @return boolean
 */
Conversation.prototype.isSentenceAdressedToMe = function(character, sentence) {


};

Conversation.prototype.interpretSentence = function(character, sentence) {

    sentence = this.preprocessSentence(sentence);

    var patterns = Conversation.SENTENCE_PATTERNS;
    for (var key in patterns) {
        var pattern = patterns[key];
        var match = pattern.regex.exec(sentence);
        if (match !== null) {
            pattern = pattern.applyMatch(match);
        }
    }
};

Conversation.prototype.preprocessSentence = function(sentence) {

    return sentence.trim().toLowerCase().replace(/[^\s\w]+/g, "").replace(/\s+/g, " ");
};
