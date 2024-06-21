package gui.views;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.io.File;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;

import common.ReviewThread;
import game.Reviewer;
import game.objects.Maze;
import game.view.MazeView;
import gui.Game;
import gui.components.Button;
import gui.components.Label;

/**
 * The class that represents view to watch player's
 * game.
 * 
 * Here player can review his last games using buttons to
 * forward or roll back the frames.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class GameViewer extends View {
    /* PANELS */
    /** footer */
    private final JPanel footerContainer = new JPanel(new GridLayout(1, 5));
    /** header */
    private final JPanel top = new JPanel(new BorderLayout());
    /** header info panel */
    private final JPanel infoContainerTop = new JPanel(new GridLayout(1, 5));
    /** header margin left */
    private final JPanel topLeft = new JPanel();
    /** header margin right */
    private final JPanel topRight = new JPanel();

    /* BUTTONS */
    /** button for the prev instruction */
    private final Button prevInstruction = new Button("Prev");
    /** button for the next instruction */
    private final Button nextInstruction = new Button("Next");
    /** button for the smooth mode */
    private final Button smoothMode = new Button("Smooth stepping");
    /** button for the gradual mode */
    private final Button gradualMode = new Button("Gradual stepping");

    /** maze to view */
    private Maze maze;
    /** thread for reading instructions */
    private ReviewThread reviewerThread;

    /**
     * Creates game GUI
     * 
     * @param game abstraction of the game
     * @param file file of the map
     */
    public GameViewer(Game game, File file) {
        super(game);

        setBackground(Color.BLACK);

        footerContainer.setOpaque(false);
        footerContainer.setBorder(BorderFactory.createEmptyBorder(0, 0, 40, 0));

        top.setOpaque(false);
        topLeft.setOpaque(false);
        topRight.setOpaque(false);
        infoContainerTop.setOpaque(false);

        topLeft.setPreferredSize(new Dimension((config.getWidth() - 600) / 2, 40));
        topRight.setPreferredSize(new Dimension((config.getWidth() - 600) / 2, 40));

        Reviewer reviewer = new Reviewer(game, file);
        maze = reviewer.getMaze();

        reviewerThread = new ReviewThread(maze, file);

        reviewerThread.start();

        MazeView mazeView = new MazeView(maze, game);

        mazeView.setPreferredSize(new Dimension(550, 550));
        container.add(mazeView);

        prevInstruction.setPreferredSize(new Dimension(100, 50));

        prevInstruction.addActionListener(e -> {
            reviewerThread.prev();
        });

        nextInstruction.addActionListener(e -> {
            reviewerThread.next();
        });

        smoothMode.addActionListener(e -> {
            smoothMode.setActive(true);
            gradualMode.setActive(false);
            reviewerThread.setSmoothStepping();
        });

        gradualMode.addActionListener(e -> {
            smoothMode.setActive(false);
            gradualMode.setActive(true);
            reviewerThread.setGradualStepping();
        });

        Label scoreText = maze.getMazeComponent().getScoreText();
        scoreText.setHorizontalAlignment(SwingConstants.CENTER);

        infoContainerTop.add(scoreText);
        infoContainerTop.add(new JLabel());
        infoContainerTop.add(maze.getMazeComponent().getKeyContainer());
        infoContainerTop.add(new JLabel());
        infoContainerTop.add(maze.getMazeComponent().getHealthContainer());

        top.add(topLeft, BorderLayout.WEST);
        top.add(topRight, BorderLayout.EAST);
        top.add(infoContainerTop, BorderLayout.CENTER);

        buttons.add(prevInstruction);
        buttons.add(nextInstruction);
        buttons.add(smoothMode);
        buttons.add(gradualMode);

        footerContainer.add(prevInstruction);
        footerContainer.add(nextInstruction);
        footerContainer.add(reviewerThread.getIndexLabel());
        footerContainer.add(smoothMode);
        footerContainer.add(gradualMode);

        selectButton(buttons.indexOf(nextInstruction));

        add(top, BorderLayout.NORTH);

        add(footerContainer, BorderLayout.SOUTH);

        gradualMode.setActive(true);

    }

    @Override
    protected void KeyArrowLeft() {
        selectPrevButton();
        game.update();
    }

    @Override
    protected void KeyArrowUp() {
    }

    @Override
    protected void KeyArrowRight() {
        selectNextButton();
        game.update();
    }

    @Override
    protected void KeyArrowDown() {
    }

    @Override
    protected void KeyEscape() {
        reviewerThread.interrupt();
        game.popView();
    }

    @Override
    protected void KeyEnter() {
        buttons.get(activeButton).doClick();
    }

    @Override
    protected void AnyKey() {
    }

}
