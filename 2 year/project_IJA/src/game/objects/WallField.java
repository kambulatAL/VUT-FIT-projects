package game.objects;

import game.common.CommonField;
import game.common.CommonMazeObject;

/**
 * Class representing wall in maze.
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class WallField extends Field {

    /**
     * Constructor of the WallField object
     * @param maze maze
     * @param x x coordinate of the field
     * @param y y coordinate of the field
     */
    public WallField(Maze maze, int x, int y) {
        super(maze, x, y);
    }

    @Override
    public boolean isEmpty() {
        return true;
    }

    @Override
    public boolean canMove() {
        return false;
    }

    @Override
    public boolean contains(CommonMazeObject obj) {
        return false;
    }

    @Override
    public CommonField nextField(Direction dir) {
        throw new UnsupportedOperationException("Unimplemented method 'nextField'");
    }

    @Override
    public void put(CommonMazeObject obj) {
        throw new UnsupportedOperationException("Unimplemented method 'put'");
    }

}
