package gui;

import common.Config;
import gui.views.MainView;
import gui.views.View;

import java.util.EmptyStackException;
import java.util.Stack;

import javax.swing.ImageIcon;
import javax.swing.JFrame;

/**
 * Class that keeps track of general data of the project
 * which has to be visible in many places
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @version 1.0
 */
public class Game extends JFrame {
    /** Config of the game */
    private final Config config = new Config();
    /** GUI pages */
    private final Stack<View> views = new Stack<>();
    /** default view that should be launched */
    private final View defaultView = new MainView(this);

    /**
     * Constructor for class Game.
     * It configures main JFrame for the game
     */
    public Game() {
        super("Pacman");
        setVisible(true); // make window visible
        setSize(config.getWidth(), config.getHeight()); // set window size
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE); // close behaviour for window
        setResizable(false); // resizable
        setLayout(null);
        setIconImage(new ImageIcon("lib/sprites/icon.png").getImage());
    }

    /**
     * Launches main window with default page in it
     */
    public void launch() {
        views.push(defaultView);

        add(getCurrentView());

        update();
    }

    /**
     * Interface updating method for better user interactions
     */
    public void update() {
        repaint();
        revalidate();
    }

    /**
     * Returns latest active view
     * 
     * @return latest active view
     */
    private View getCurrentView() {
        try {
            return views.peek();
        } catch (EmptyStackException e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Checks whether last view in a list is a default one
     * 
     * @return returns true if the first view is a default
     */
    private boolean isEmpty() {
        return getCurrentView().getClass() == defaultView.getClass();
    }

    /**
     * Swaps last you with a given one
     * 
     * @param view new view
     */
    public void swapView(View view) {
        View current = views.pop();
        views.push(view);

        current.cleanup();
        remove(current);
        add(view);

        update();
    }

    /**
     * Opens up new view (Adds it to the stack)
     * 
     * @param view new view
     */
    public void pushView(View view) {
        View current = getCurrentView();
        views.push(view);
        current.cleanup();
        remove(current);
        add(getCurrentView());

        update();
    }

    /**
     * Pops last view if it's not default one
     */
    public void popView() {
        if (isEmpty())
            return;

        View current = views.pop();
        current.cleanup();
        remove(current);
        add(getCurrentView());

        update();
    }
}
