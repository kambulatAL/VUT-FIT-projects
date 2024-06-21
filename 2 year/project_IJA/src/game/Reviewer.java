package game;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.NoSuchElementException;
import java.util.Optional;
import java.util.Scanner;

import common.Config;
import game.objects.Maze;
import gui.Game;

/**
 * A class representing view of the game
 *
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class Reviewer {
    /** list of instructions to review */
    private final List<String> instructions = new ArrayList<>();
    /** config of the game */
    private final Config config = new Config();
    /** maze that is being reviewed */
    private Maze maze;

    /**
     * Creates reviewer for the maze to be able to review it
     * 
     * @param game game to which this review is bound to
     * @param reviewFile map file for the review
     */
    public Reviewer(Game game, File reviewFile) {
        try {
            Scanner scanner = new Scanner(reviewFile);
            List<File> maps = config.getMaps();
            String filename = scanner.nextLine();
            Optional<File> mapResult = maps.stream().filter(map -> map.getName().equals(filename)).findFirst();

            if (!mapResult.isPresent()) {
                scanner.close();
                throw new FileNotFoundException("Unknown map name");
            }


            MazeConfigure cfg = new MazeConfigure(mapResult.get(), true);
            this.maze = cfg.getMaze();

            while (scanner.hasNext())
                instructions.add(scanner.nextLine());
                
            scanner.close();

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (NoSuchElementException e) {
            e.printStackTrace();
        }
        
    }

    /**
     * getter for instructions
     * 
     * @return list of instructions
     */
    public List<String> getInstructions() {
        return new ArrayList<>(this.instructions);
    }

    /**
     * getter for the maze
     * 
     * @return maze
     */
    public Maze getMaze() {
        return this.maze;
    }
}
