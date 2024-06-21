package common;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;

import javax.swing.SwingConstants;

import game.objects.Maze;
import game.objects.PacmanObject;
import gui.components.Label;

/**
 * Class that creates thread in order to properly review last player's games.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class ReviewThread extends Thread {

    private final Object lock = new Object();

    private boolean isForward = true;

    private Maze maze;
    private boolean isAuto = false;
    private int index;
    private int prevLives;

    private final Label instructionCounterLabel = new Label("");

    private final List<String> instructions = new ArrayList<>();
    private final List<Integer> liveChanges = new ArrayList<>();

    /**
     * Constructor of the review thread
     * @param maze maze
     * @param scene file of the scene
     */
    public ReviewThread(Maze maze, File scene) {
        this.maze = maze;

        this.prevLives = maze.getPacman().getLives();

        try {
            Scanner scanner = new Scanner(scene);

            scanner.nextLine();

            while (scanner.hasNextLine()) {
                instructions.add(scanner.nextLine());
            }

            scanner.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        updateText();

    }

    public void run() {

        try {
            while (true) {
                if (isAuto) {
                    Thread.sleep(200);
                } else {
                    pause();
                }

                if (isForward) {
                    next();
                } else {
                    prev();
                }
            }

        } catch (InterruptedException | IllegalArgumentException e) {
        }
    }

    /**
     * Updates breadcrumbs for the instructions
     */
    private void updateText() {
        instructionCounterLabel.setHorizontalAlignment(SwingConstants.CENTER);
        instructionCounterLabel.setText((index + 1) + "/" + instructions.size());
    }

    /**
     * Returns label that contains text for GUI, that informs player
     * 
     * @return Returns label
     */
    public Label getIndexLabel() {
        return this.instructionCounterLabel;
    }

    /**
     * Stops thread in order to allow player
     * to control review manually
     */
    private void pause() {
        try {
            synchronized (lock) {
                lock.wait();
            }
        } catch (InterruptedException | IllegalArgumentException e) {

        }
    }

    /**
     * Continues the thread if user wants it to be automatic
     */
    private void release() {
        synchronized (lock) {
            lock.notify();
        }
    }

    /**
     * Sets mode fro 'smooth stepping' review, which is an automatic one.
     */
    public void setSmoothStepping() {
        release();
        isAuto = true;
    }

    /**
     * Sets mode for 'gradual stepping' review, which is manual one.
     */
    public void setGradualStepping() {
        isAuto = false;
    }

    /**
     * Calculates next index of the instruction
     */
    public void next() {
        synchronized (lock) {
            isForward = true;

            if (index >= instructions.size() - 1)
                return;

            maze.processInstruction(instructions.get(index++), false);

            updateText();

            PacmanObject pacman = maze.getPacman();

            if (prevLives > pacman.getLives()) {
                prevLives = pacman.getLives();
                liveChanges.add(index - 1);
            }
        }
    }

    /**
     * Calculates previous index of the instruction
     */
    public void prev() {
        synchronized (lock) {

            isForward = false;

            if (index <= 0)
                return;

            PacmanObject pacman = maze.getPacman();

            maze.processInstruction(instructions.get(--index), true);

            updateText();

            if (prevLives > pacman.getLives()) {
                pacman.undoHit();
                prevLives = pacman.getLives();
            } else if (liveChanges.contains(Integer.valueOf(index))) {
                pacman.undoHit();
                prevLives = pacman.getLives();
                liveChanges.remove(Integer.valueOf(index));
            }

        }
    }
}
