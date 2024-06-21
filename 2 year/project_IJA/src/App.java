import gui.Game;

/**
 * Starting point of the game
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class App {

    /**
     * Main function
     * 
     * @param args command line arguments
     * @throws Exception basic exception class
     */
    public static void main(String[] args) throws Exception {
        new Game().launch(); // creates object of game and launches it
    }
}
