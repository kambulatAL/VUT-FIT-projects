package gui.views;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.io.File;
import java.util.List;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingConstants;
import game.objects.Maze;
import gui.Game;
import gui.components.Button;
import gui.components.Label;

/**
 * This class represents view when player wins the game
 * 
 * Here player can see his game results, name of the map.
 * He can also toggle replay of this game right away or just exit the view.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class WinnerView extends View {
    /* PANELS */
    /** centered grid */
    private final JPanel center = new JPanel(new GridLayout(10, 1));
    /** left margin */
    private final JPanel leftMargin = new JPanel();
    /** right margin */
    private final JPanel rightMargin = new JPanel();
    /** button container */
    private final JPanel buttonContainer = new JPanel(new GridLayout(1, 2, 10, 10));
    /** footer panel */
    private final JPanel footer = new JPanel();

    /* BUTTONS */
    /** button replay */
    private final Button buttonReplay = new Button("Watch replay");
    /** button okay */
    private final Button buttonOkay = new Button("Okay");

    /** replay logging filer */
    private final List<File> replayFiles = config.getFiles("data/replays");

    /**
     * Creates winner view for the user when pacman wins the game
     * 
     * @param game game
     * @param maze maze where player played
     */
    public WinnerView(Game game, Maze maze) {
        super(game, maze, "CONGRATULATIONS!!!");

        container.setLayout(new BorderLayout());
        center.setOpaque(false);

        leftMargin.setPreferredSize(new Dimension(350, 10));
        rightMargin.setPreferredSize(new Dimension(350, 10));

        leftMargin.setOpaque(false);
        rightMargin.setOpaque(false);

        container.add(leftMargin, BorderLayout.WEST);
        container.add(rightMargin, BorderLayout.EAST);

        Label titleText = new Label("Winner, winner - chicken dinner!", 15);
        Label scoreText = maze.getMazeComponent().getScoreText();
        Label mapText = new Label("Map: " + maze.getMazeName());

        titleText.setHorizontalAlignment(SwingConstants.CENTER);
        scoreText.setHorizontalAlignment(SwingConstants.CENTER);
        mapText.setHorizontalAlignment(SwingConstants.CENTER);

        center.add(titleText);
        center.add(scoreText);
        center.add(mapText);

        center.add(new JLabel());
        center.add(new JLabel());
        center.add(new JLabel());
        center.add(new JLabel());
        center.add(new JLabel());

        buttonContainer.setOpaque(false);

        center.add(buttonContainer);

        footer.setPreferredSize(new Dimension(10, 100));
        footer.setOpaque(false);

        container.add(center, BorderLayout.CENTER);
        container.add(footer, BorderLayout.SOUTH);

        buttonReplay.addActionListener(e -> {
            game.pushView(new GameViewer(game, replayFiles.get(replayFiles.size() - 1)));
        });

        buttonOkay.addActionListener(e -> {
            KeyEscape();
        });

        buttons.add(buttonReplay);
        buttons.add(buttonOkay);

        for (Button button : buttons) {
            buttonContainer.add(button);
        }

        selectButton(buttons.size() - 1);

        setBackground(Color.BLACK);
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
        game.popView();
        game.swapView(new StartGameView(game, config.getMaps().indexOf(maze.getMazeFile())));
    }

    @Override
    protected void KeyEnter() {
        buttons.get(activeButton).doClick();
    }

    @Override
    protected void AnyKey() {
    }

}
