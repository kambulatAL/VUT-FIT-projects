package game.common;

/**
 * An interface represents an object that can be placed on the CommonField of a
 * maze (Maze). The interface defines operations for moving an object to
 * adjacent fields (moving an object). Objects can move between fields only in
 * the specified direction (Field.Direction) and by one step (one field) at a
 * time.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public interface CommonMazeObject {
    /**
     * Checks whether the object can be moved in the specified direction.
     * 
     * @param dir The direction to move the object to.
     * @return method success
     */
    public boolean canMove(CommonField.Direction dir);

    /**
     * Moves the object onto the array in the specified direction if possible.
     * 
     * @param dir        The direction to move the object to.
     * @param isInverted Inverts pacman's direction
     * @return method success
     */
    public boolean move(CommonField.Direction dir, boolean isInverted);

    /**
     * Binds object to a specific field in a maze
     * 
     * @param field field to which we bind our object to
     */
    public void setField(CommonField field);

    /**
     * Returns the box object on which the object is placed.
     * 
     * @return The field on which the object is placed.
     */
    public CommonField getField();

    /**
     * Checks if the object is represented by a Pacman stick figure.
     * 
     * @return true if the object is a pacman
     */
    public boolean isPacman();

    /**
     * Returns the object's current health count.
     * 
     * @return Number of lives.
     */
    public int getLives();

    /**
     * Returns id of the maze object
     * 
     * @return object id
     */
    public int getId();

    /**
     * Returns column index of the field in a maze
     * 
     * @return column index
     */
    public int getX();

    /**
     * Returns row index of the field in a maze
     * 
     * @return row index
     */
    public int getY();
}
