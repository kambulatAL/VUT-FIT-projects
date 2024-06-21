package common;

import game.common.CommonField;

/**
 * Class that creates new thread for a pacman (When A* algorithm is used).
 * 
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class PacmanThread extends Thread {
    private CommonField targetField;

    /**
     * Constructs thread for the Pacman object
     * @param targetField field of the maze
     */
    public PacmanThread(CommonField targetField) {
        this.targetField = targetField;
    }

    /**
     * run A* algorithm in a pacman thread to react on the mouse click
     */
    public void run() {
        try {
            AStar algorithm = new AStar(targetField);
            algorithm.startAStar();
        } catch (InterruptedException | IllegalArgumentException e) {

        }
    }
}
