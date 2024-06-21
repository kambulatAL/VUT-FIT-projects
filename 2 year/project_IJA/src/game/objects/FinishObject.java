package game.objects;

import game.common.CommonField;
import game.common.CommonField.Direction;

/**
 * Class representing finish object
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class FinishObject extends MazeObject {
    /**
     * Initialize the finish objects(gates)
     * @param field represents a field of the maze
     */
    public FinishObject(CommonField field) {
        super(field);
    }

    @Override
    public boolean isPacman() {
        return false;
    }

    @Override
    public int getLives() {
        throw new UnsupportedOperationException("Unimplemented method 'getLives'");
    }

    @Override
    public boolean move(Direction dir, boolean isInverted) {
        throw new UnsupportedOperationException("Unimplemented method 'move'");
    }

}
