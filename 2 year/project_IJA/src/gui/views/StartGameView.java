package gui.views;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Image;
import java.io.File;
import java.util.Iterator;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;

import common.GhostThread;
import game.MazeConfigure;
import game.objects.GhostObject;
import game.objects.Maze;
import game.view.MazeView;
import gui.Game;
import gui.components.Button;
import gui.components.Label;

/**
 * This class represents view where player can
 * choose map from the list of all available maps
 * in the game.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class StartGameView extends View {
    /* PANELS */
    /** map content */
    private final JPanel mapContent = new JPanel();
    /** map breadcrumbs */
    private final JLabel mapBreadcrumbs = new JLabel();
    /** button container */
    private final JPanel buttonContainer = new JPanel(new GridLayout(1, 6, 5, 5));
    /** left margin */
    private final JLabel leftBorder = new JLabel();
    /** right margin */
    private final JLabel rightBorder = new JLabel();
    /** bottom footer */
    private final JLabel bottomPanel = new JLabel();
    /** map panel */
    private final JPanel map = new JPanel();
    /** map info */
    private final JPanel mapInfo = new JPanel(new GridLayout(3, 1));
    /** map info stats */
    private final JPanel mapInfoStats = new JPanel(new GridLayout(1, 5));

    /* BUTTONS */
    /** button next */
    private final Button buttonNext = new Button("Next");
    /** button prev */
    private final Button buttonPrev = new Button("Previous");
    /** button play */
    private final Button buttonPlay = new Button("Play");
    /** map index */
    private int mapIndex = 0;
    /** maze */
    private Maze maze;

    /**
     * View where user can pick the game
     * 
     * @param game game
     * @param mapIndex map index
     */
    public StartGameView(Game game, int mapIndex) {
        super(game, "Start game");

        this.mapIndex = mapIndex;

        setBackground(Color.BLACK);

        mapContent.setOpaque(false);

        container.setLayout(new BorderLayout(2, 2));

        buttonContainer.setOpaque(false);

        Border buttonContainerBorder = getBorder();
        Border buttonContainerMargin = new EmptyBorder(0, 0, 40, 0);
        buttonContainer.setBorder(new CompoundBorder(buttonContainerBorder, buttonContainerMargin));

        buttonNext.setPreferredSize(new Dimension(100, 50));
        buttonPrev.setPreferredSize(new Dimension(100, 50));
        buttonPlay.setPreferredSize(new Dimension(100, 50));

        buttons.add(buttonPrev);
        buttons.add(buttonNext);
        buttons.add(buttonPlay);

        for (Button button : buttons) {
            buttonContainer.add(button);
        }

        mapBreadcrumbs.setText((mapIndex + 1) + "/" + config.getMaps().size());
        mapBreadcrumbs.setForeground(Color.WHITE);
        mapBreadcrumbs.setFont(config.getFont("emulogic.ttf", 10f));

        buttonContainer.add(new JLabel());
        buttonContainer.add(mapBreadcrumbs);
        buttonContainer.add(new JLabel());
        buttonContainer.add(buttonPlay);

        selectButton(buttons.size() - 1);

        leftBorder.setPreferredSize(new Dimension(20, 20));
        rightBorder.setPreferredSize(new Dimension(20, 20));

        updateMapContent(config.getMaps().get(mapIndex));

        buttonPlay.addActionListener(e -> {
            game.pushView(new GameView(maze, game));
            for (GhostObject ghost : maze.ghosts())
                new GhostThread(ghost).start();
        });

        buttonNext.addActionListener(e -> {
            getNextMap();
            game.update();
        });

        buttonPrev.addActionListener(e -> {
            getPrevMap();
            game.update();
        });

        mapContent.setBackground(new Color(0, 0, 0, 0));
        mapContent.setLayout(new GridLayout(1, 2, 10, 10));
        map.setOpaque(false);
        mapInfo.setOpaque(false);
        mapInfoStats.setOpaque(false);

        mapContent.setPreferredSize(new Dimension(config.getWidth(), 20));

        container.add(buttonContainer, BorderLayout.SOUTH);
        container.add(leftBorder, BorderLayout.WEST);
        container.add(rightBorder, BorderLayout.EAST);
        container.add(bottomPanel, BorderLayout.NORTH);
        container.add(mapContent, BorderLayout.CENTER);
    }

    /**
     * Default constructor
     * 
     * @param game game
     */
    public StartGameView(Game game) {
        this(game, 0);
    }

    /**
     * Retrieves next map from the list of maps
     */
    private void getNextMap() {
        mapIndex = (mapIndex + 1) % config.getMaps().size();
        updateMapContent(config.getMaps().get(mapIndex));
    }

    /**
     * Retrieves previous map from the list of maps
     */
    private void getPrevMap() {
        mapIndex = (mapIndex-- > 0 ? mapIndex : config.getMaps().size() - 1);
        updateMapContent(config.getMaps().get(mapIndex));
    }

    /**
     * Updates map content, its description, difficulty and other details
     * relevant to the map.
     * 
     * @param mapFile
     */
    private void updateMapContent(File mapFile) {

        mapBreadcrumbs.setText((mapIndex + 1) + "/" + config.getMaps().size());

        MazeConfigure mazeConfig = new MazeConfigure(mapFile, false);
        maze = mazeConfig.getMaze();
        JPanel mazePanel = new MazeView(maze, game);

        mapInfoStats.removeAll();

        JPanel difficultyContainer = new JPanel(new GridLayout(1, 3));

        difficultyContainer.setOpaque(false);

        Iterator<GhostObject> ghostIterator = maze.ghosts().iterator();

        while (ghostIterator.hasNext()) {
            Image ghostImage = new ImageIcon("lib/sprites/game/ghost.png").getImage();
            JLabel ghostLabel = new JLabel(new ImageIcon(ghostImage.getScaledInstance(30, 30, Image.SCALE_SMOOTH)));
            difficultyContainer.add(ghostLabel);
            ghostIterator.next();
        }

        mapInfoStats.add(new Label("Difficulty:"));
        mapInfoStats.add(difficultyContainer);
        mapInfoStats.add(new JLabel());
        mapInfoStats.add(new JLabel());
        mapInfoStats.add(new JLabel());

        mapInfo.removeAll();
        mapInfo.add(mapInfoStats);
        String[] data = { "This is demo description" };
        JList<String> story = new JList<>(data);
        story.setOpaque(false);
        story.setBackground(new Color(0, 0, 0, 0));
        story.setForeground(Color.WHITE);
        story.setFocusable(false);

        mapInfo.add(story);

        mapContent.removeAll();

        mapContent.add(mapInfo);
        mapContent.add(mazePanel);

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
    }

    @Override
    protected void KeyEnter() {
        buttons.get(activeButton).doClick();
    }

    @Override
    protected void AnyKey() {
    }
}
