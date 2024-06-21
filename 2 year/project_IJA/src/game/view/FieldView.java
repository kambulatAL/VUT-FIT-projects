package game.view;

import java.util.ArrayList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JPanel;

import common.PacmanThread;
import game.common.CommonField;
import game.common.CommonMazeObject;
import game.common.Observable;
import game.objects.FinishObject;
import game.objects.GhostObject;
import game.objects.KeyObject;
import game.objects.PacmanObject;
import game.objects.PointObject;
import gui.Game;
import gui.views.LoserView;
import gui.views.WinnerView;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

/**
 * A class representing the graphical form of a field. Implements
 * Observable.Observer, can be notified of a field's state change.
 * 
 * @author Turtysia Oleksandr (xturyt00)
 * @version 1.0
 */
public class FieldView extends JPanel implements Observable.Observer {
    /** Objects that are in a field */
    private final List<ComponentView> objects = new ArrayList<>();
    /** field to which this field view is bound */
    private CommonField field;
    /** binds field view to the game */
    private Game game;
    /** Pacman thread for A* walking */
    private static PacmanThread thread;

    /**
     * Initialize the view of the field
     * @param field represents a field of the maze
     * @param game Game object
     */
    public FieldView(CommonField field, Game game) {
        this.field = field;
        this.game = game;

        this.field.addObserver(this);
        setOpaque(false);

        updateView();

        addMouseListener(new MouseAdapter() {
            @Override
            public void mouseClicked(MouseEvent e) {
                if (thread == null) {
                    thread = new PacmanThread(field);
                    thread.start();
                } else {
                    thread.interrupt();
                    thread = new PacmanThread(field);
                    thread.start();
                }

            }

            @Override
            public void mouseEntered(MouseEvent e) {
                setBorder(BorderFactory.createLineBorder(Color.WHITE));
            }

            @Override
            public void mouseExited(MouseEvent e) {
                setBorder(null);
            }
        });
    }

    /**
     * Updates specific object that was changed
     */
    private void updateView() {
        objects.clear();
        for (CommonMazeObject obj : field.get()) {
            if (obj.isPacman()) {
                this.objects.add(new PacmanView(this, obj));
            } else if (obj instanceof GhostObject) {
                this.objects.add(new GhostView(this));
            } else if (obj instanceof KeyObject) {
                this.objects.add(new KeyView(this));
            } else if (obj instanceof FinishObject) {
                this.objects.add(new FinishView(this));
            } else if (obj instanceof PointObject) {
                this.objects.add(new PointView(this));
            } else {
                System.out.println("Something is wrong...");
            }
        }

    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        this.objects.forEach((v) -> {
            v.paintComponent(g);
        });
    }

    @Override
    public void update(Observable f) {
        updateView();

        PacmanObject pacman = field.getMaze().getPacman();

        if (pacman.getLives() <= 0) {
            game.swapView(new LoserView(game, field.getMaze()));
        } else if (pacman.getFinished()) {
            game.swapView(new WinnerView(game, field.getMaze()));
        } else {
            game.update();
        }
    }

}
