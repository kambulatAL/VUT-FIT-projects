package game.objects;

import game.common.CommonField;
import game.common.CommonField.Direction;

/**
 * Class representing key object
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class KeyObject extends MazeObject {

    /**
     * Constructor for the Key object
     * @param field field of the maze
     */
    public KeyObject(CommonField field) {
        super(field);
    }

    /**
     * Checks if object is the Pacman object
     * @return boolean value of the checking
     */
    @Override
    public boolean isPacman() {
        return false;
    }

    /**
     * throws an UnsupportedOperationException, because key object doesn't have lives
     */
    @Override
    public int getLives() {
        throw new UnsupportedOperationException("Unimplemented method 'getLives'");
    }

    /**
     * throws an UnsupportedOperationException, because key object doesn't have move method
     * @param dir        The direction to move the object to.
     * @param isInverted Inverts pacman's direction
     */
    @Override
    public boolean move(Direction dir, boolean isInverted) {
        throw new UnsupportedOperationException("Unimplemented method 'move'");
    }

}
