package gui.views;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JPanel;

import common.Config;
import game.objects.Maze;
import gui.Game;
import gui.components.Button;
import gui.components.Title;

/**
 * Abstract class that implements base behaviour
 * of pages in the interface. By default it binds itself to a
 * specific game object to access general data.
 * 
 * By default each View can have buttons (if necessary) and it keeps
 * track of its active index.
 * 
 * @see Game
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public abstract class View extends JPanel implements KeyListener {

    // KEY CONSTANTS
    /** constant for left key */
    private final int ArrowLeft = 37;
    /** constant for up key */
    private final int ArrowUp = 38;
    /** constant for right key */
    private final int ArrowRight = 39;
    /** constant for down key */
    private final int ArrowDown = 40;
    /** constant for enter key */
    private final int KeyEnter = 10;
    /** constant for esc key */
    private final int KeyEsc = 27;
    /** constant for key char up */
    private final int KeyUp = (int) 'W';
    /** constant for key char left */
    private final int KeyLeft = (int) 'A';
    /** constant for key char down */
    private final int KeyDown = (int) 'S';
    /** constant for key char right */
    private final int KeyRight = (int) 'D';

    /** buttons */
    protected final List<Button> buttons = new ArrayList<>();
    /** config for the game */
    protected final Config config = new Config();
    /** container for the content */
    protected final JPanel container = new JPanel();

    /** maze for the game */
    protected Maze maze;
    /** game */
    protected Game game;
    /** title in header */
    private Title title;

    /** active button index */
    protected int activeButton = 0;

    /**
     * Constructor that sets up general layout of the page
     */
    private View() {
        setSize(config.getWidth() - 16, config.getHeight());
        setVisible(true);
        setLayout(new BorderLayout());

        container.setOpaque(false);
        container.setBackground(new Color(0, 0, 0, 0));

        add(container, BorderLayout.CENTER);
    }

    /**
     * Constructor that also creates title of the page (at its header)
     * 
     * @param title page title
     */
    private View(String title) {
        this();
        this.title = new Title(title);
        add(this.title, BorderLayout.NORTH);
    }

    /**
     * Public constructor for the case when maze and title are
     * available
     * 
     * @param game  object of the game
     * @param maze  maze object
     * @param title page title
     */
    public View(Game game, Maze maze, String title) {
        this(title);
        this.game = game;
        this.maze = maze;
    }

    /**
     * Public constructor for the case when only maze is
     * available. The name of the page is taken as the name of the maze.
     * 
     * @param game object of the game
     * @param maze maze object
     */
    public View(Game game, Maze maze) {
        this(maze.getMazeName());
        this.game = game;
        this.maze = maze;
    }

    /**
     * Public constructor for the case when no maze is
     * available, but the title is.
     * 
     * @param game  object of the game
     * @param title page title
     */
    public View(Game game, String title) {
        this(title);
        this.game = game;
    }

    /**
     * Public constructor for the case when no extra data is
     * available. (It is used for main page)
     * 
     * @param game object of the game
     */
    public View(Game game) {
        this();
        this.game = game;
    }

    /**
     * Selects next active index
     */
    protected void selectNextButton() {
        selectButton((activeButton + 1) % buttons.size());
    }

    /**
     * Selects previous active index
     */
    protected void selectPrevButton() {
        selectButton(activeButton - 1 < 0 ? buttons.size() - 1 : activeButton - 1);
    }

    /**
     * Selects button by index
     * 
     * @param index index of the button
     */
    protected void selectButton(int index) {
        buttons.get(activeButton).setSelect(false);

        activeButton = index;
        buttons.get(activeButton).setSelect(true);
    }

    /**
     * Removes listeners from the view (if they exist)
     */
    public void cleanup() {
        removeKeyListener(this);
    }

    @Override
    public void keyPressed(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEnter:
                KeyEnter();
                break;
            case KeyEsc:
                KeyEscape();
                break;
            case KeyLeft:
            case ArrowLeft:
                KeyArrowLeft();
                break;
            case KeyUp:
            case ArrowUp:
                KeyArrowUp();
                break;
            case KeyRight:
            case ArrowRight:
                KeyArrowRight();
                break;
            case KeyDown:
            case ArrowDown:
                KeyArrowDown();
                break;
        }
    }

    @Override
    public void keyTyped(KeyEvent e) {
    }

    @Override
    public void keyReleased(KeyEvent e) {
        AnyKey();
    }

    @Override
    public void addNotify() {
        super.addNotify();
        addKeyListener(this);

        setFocusable(true);
        requestFocusInWindow();
    }

    /**
     * Special method that will be called when user
     * pressed arrow left or key 'a'
     */
    protected abstract void KeyArrowLeft();

    /**
     * Special method that will be called when user
     * pressed arrow up or key 'w'
     */
    protected abstract void KeyArrowUp();

    /**
     * Special method that will be called when user
     * pressed arrow right or key 'd'
     */
    protected abstract void KeyArrowRight();

    /**
     * Special method that will be called when user
     * pressed arrow down or key 's'
     */
    protected abstract void KeyArrowDown();

    /**
     * Special method that will be called when user
     * pressed key 'Esc'
     */
    protected abstract void KeyEscape();

    /**
     * Special method that will be called when user
     * pressed key 'Enter'
     */
    protected abstract void KeyEnter();

    /**
     * Special method that will be called when user
     * pressed any key
     */
    protected abstract void AnyKey();
}
