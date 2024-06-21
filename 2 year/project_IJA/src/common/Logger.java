package common;

import game.common.CommonField;
import game.objects.GhostObject;
import game.objects.Maze;
import game.objects.MazeObject;
import game.objects.PacmanObject;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Class that implements logger in the game.
 * 
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class Logger {
    private Maze maze;
    private static boolean isFileLogs = false;
    private static final List<String> logs = Collections.synchronizedList(new ArrayList<>());

    /**
     * Constructs Logger object
     * @param obj Maze object
     */
    public Logger(MazeObject obj) {
        this.maze = obj.getField().getMaze();
    }

    /**
     * Logs move of the given maze object
     * 
     * @param dir direction where the object has moved
     * @param obj the object itself
     */
    public static void log(CommonField.Direction dir, MazeObject obj) {
        synchronized (logs) {
            if (obj.getField().getMaze().getReview())
                return;

            if (obj instanceof PacmanObject) {
                logs.add("PACMAN " + dir);
            } else if (obj instanceof GhostObject) {
                logs.add("GHOST" + obj.getId() + " " + dir);
            }
        }
    }

    /**
     * When logging is done, this function takes all the logs and
     * puts them into a new file
     * 
     * @param fileName name of the new file
     */
    public void print_logs(String fileName) {
        if (this.maze.getReview())
            return;

        if (isFileLogs)
            return;

        logs.add(0, fileName);
        LocalDateTime now = LocalDateTime.now();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("-yyyy-MM-dd-HH-mm-ss-");
        String formattedDateTime = now.format(formatter);

        File file = new File("data/replays/replay" + formattedDateTime + this.maze.getMazeFile().getName());
        try {
            FileWriter fileWriter = new FileWriter(file);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            for (String str : logs) {
                bufferedWriter.write(str);
                bufferedWriter.newLine();
            }
            bufferedWriter.close();
            fileWriter.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Clears out previous logs
     */
    public void logs_clear() {
        if (this.maze.getReview())
            return;

        isFileLogs = false;
        logs.clear();
    }
}
