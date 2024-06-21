package gui.views;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;

import java.io.File;
import java.util.List;

import javax.swing.JPanel;

import gui.Game;
import gui.components.Button;
import gui.components.Label;

/**
 * This class represents view that lists
 * all the player's recent games.
 * 
 * In this view user can choose his last game and rewatch it as a video.
 * He is provided with various handlers and modes in order
 * to control entire flow of his game.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class LoadGameView extends View {

    /** list footer */
    private final JPanel listFooter = new JPanel();
    /** replay files */
    private final List<File> replayFiles = config.getFiles("data/replays");
    /** map's list */
    private final JPanel gameList = new JPanel(new GridLayout(8, 1));

    /**
     * Create loader for the game
     * 
     * @param game current game
     */
    public LoadGameView(Game game) {
        super(game, "Load game");

        setBackground(Color.BLACK);

        gameList.setOpaque(false);
        listFooter.setOpaque(false);

        container.add(gameList);

        for (File file : replayFiles) {

            Button fileInfoButton = new Button(file.getName());

            fileInfoButton.setLayout(new BorderLayout(10, 10));
            fileInfoButton.setPreferredSize(new Dimension(400, 50));
            fileInfoButton.setOpaque(false);

            fileInfoButton.addActionListener(e -> {
                game.pushView(new GameViewer(game, file));
            });

            gameList.add(fileInfoButton);
            buttons.add(fileInfoButton);
        }

        if (replayFiles.size() == 0) {
            gameList.add(new Label("List of your games is empty for now..."));
        }

        add(listFooter, BorderLayout.SOUTH);

        if (buttons.size() > 0) {
            selectButton(activeButton);
        }

    }

    @Override
    protected void KeyArrowLeft() {
    }

    @Override
    protected void KeyArrowUp() {
        selectPrevButton();
        game.update();
    }

    @Override
    protected void KeyArrowRight() {
    }

    @Override
    protected void KeyArrowDown() {
        selectNextButton();
        game.update();
    }

    @Override
    protected void KeyEscape() {
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
