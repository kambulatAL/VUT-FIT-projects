package common;

import game.objects.GhostObject;

/**
 * Class that creates another thread for the ghost.
 * 
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class GhostThread extends Thread {
    private GhostObject ghost;

    /**
     * Constructs a thread for a ghost
     * @param ghost Ghost object
     */
    public GhostThread(GhostObject ghost) {
        this.ghost = ghost;
    }

    /**
     * Runs Ghost moving in an infinite loop
     */
    public void run() {
        try {
            this.ghost.startMoving();
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
    }

}
