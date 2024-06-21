package game.common;

import game.objects.GhostObject;

import java.util.List;

/**
 * An interface representing a game (maze) that works with CommonMazeObject and
 * CommonField fields. Fields are placed in a grid whose dimensions are fixed.
 * The grid representing the maze is initialized when an instance of the
 * corresponding class is instantiated. The creation of the game (instance) is
 * controlled through an object of the MazeConfigure class,
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public interface CommonMaze {
    /**
     * The method returns the CommonField according to the specified position.
     * 
     * @param x The column the field is on
     * @param y The row the field is on
     * @return Field found. Returns null if the specified position is outside the
     *         range of the board.
     */
    public CommonField getField(int x, int y);

    /**
     * Returns the number of game rows.
     * 
     * @return rows cound
     */
    public int numRows();

    /**
     * Returns the number of game cols.
     * 
     * @return cols cound
     */
    public int numCols();

    /**
     * Returns a list of all ghosts in the maze.
     * 
     * @return list of all ghosts
     */
    public List<GhostObject> ghosts();

    /**
     * Converts string into valid maze representation
     * 
     * @param line instruction
     */
    public void parseLine(String line);

    /**
     * Gets pause state of the maze
     * 
     * @return returns true if maze is paused, otherwise false
     */
    public boolean getPause();

    /**
     * Generates new unique id when method is called
     * 
     * @return returns generated id
     */
    public int nextId();
}
