package game.objects;

import game.common.CommonField;
import game.common.CommonField.Direction;
import game.common.CommonMazeObject;

/**
 * Abstract class representing all maze objects (their general behaviour)
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public abstract class MazeObject implements CommonMazeObject {
    /** field object of the maze*/
    public CommonField field;

    /** id of the maze object*/
    private int id;
    /** x coordinate of the maze object*/
    private int x;
    /** y coordinate of the maze object*/
    private int y;

    /**
     * Constructs maze object
     * @param field field object of the maze
     */
    public MazeObject(CommonField field) {
        this.field = field;
        this.x = field.getX();
        this.y = field.getY();
        this.id = field.getMaze().nextId();
    }

    /**
     * Checks if maze object can move in a specified direction
     * @param dir The direction to move the object to.
     */
    @Override
    public boolean canMove(Direction dir) {
        throw new UnsupportedOperationException("Unimplemented method 'canMove'");
    }

    /**
     * Getter for the x coordinate
     * @return x coord
     */
    @Override
    public int getX() {
        return this.x;
    }

    /**
     * Getter for the y coordinate
     * @return y coord
     */
    @Override
    public int getY() {
        return this.y;
    }

    /**
    * Getter for the field object
     */
    @Override
    public CommonField getField() {
        return field;
    }

    /**
     * setter for the field object
     * @param field field to which we bind our object to
     */
    @Override
    public void setField(CommonField field) {
        this.field = field;
    }

    /**
     * getter for the id of the maze object
     * @return id
     */
    @Override
    public int getId() {
        return this.id;
    }

}
