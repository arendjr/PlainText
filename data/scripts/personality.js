
/**
 * Constructor.
 *
 * This is the main class for the Personality engine.
 *
 * @param object The game object which should be enhanced by having a personality.
 */
function Personality(object) {

    this.conversation = new Conversation(this);
}
