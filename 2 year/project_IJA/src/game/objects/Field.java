package game.objects;

import game.common.AbstractObservableField;
import game.common.CommonMazeObject;

import java.util.ArrayList;
import java.util.List;

/**
 * Abstract class representing every field in the maze
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public abstract class Field extends AbstractObservableField {
    /**
     * represents the whole maze of the game
     */
    protected Maze maze;
    /**
     * x position of the field
     */
    protected int x;
    /**
     * y position of the field
     */
    protected int y;

    /**
     *List of the objects that are bound to this field
     */
    protected final List<CommonMazeObject> objects = new ArrayList<>();

    /**
     * Initialize the Field object
     * @param maze represents the whole maze of the game
     * @param x x coordinate of the field
     * @param y y coordinate of the field
     */
    public Field(Maze maze, int x, int y) {
        this.maze = maze;
        this.x = x;
        this.y = y;
    }

    @Override
    public void remove(CommonMazeObject obj) {
        if (this.objects.isEmpty())
            return;

        this.objects.remove(obj);
        this.notifyObservers();

    }

    @Override
    public int getX() {
        return this.x;
    }

    @Override
    public int getY() {
        return this.y;
    }

    @Override
    public List<CommonMazeObject> get() {
        return objects;
    }

    @Override
    public Maze getMaze() {
        return this.maze;
    }

}
