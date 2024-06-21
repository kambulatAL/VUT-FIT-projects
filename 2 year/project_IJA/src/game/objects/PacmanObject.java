package game.objects;

import java.util.EmptyStackException;
import java.util.Stack;

import common.Logger;
import game.common.CommonField;
import game.common.CommonMazeObject;

/**
 * Class representing Pacman object. It also logs the
 * entire game here.
 * 
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class PacmanObject extends MazeObject {

    private static final int MAX_HEALTH = 3; // TODO put it in config

    private boolean hasKey = false;
    private boolean isFinish = false;
    private int lives = MAX_HEALTH;
    private int score = 0;
    private CommonField.Direction lookAt;
    private Logger logger;

    private final Stack<PointObject> points = new Stack<>();
    private KeyObject key;

    /**
     * Constructor of the pacman object
     * @param field field of the maze object
     */
    public PacmanObject(CommonField field) {
        super(field);
        this.logger = new Logger(this);
    }

    @Override
    public boolean isPacman() {
        return true;
    }

    @Override
    public boolean canMove(CommonField.Direction dir) {
        return field.nextField(dir) instanceof PathField;
    }

    /**
     * Eats the point from the field where
     * pacman stands on
     * 
     * @param field given field
     */
    public void eatPoint(PathField field) {
        if (field.objects.isEmpty())
            return;

        CommonMazeObject point = field.objects.get(0);

        if (point instanceof PointObject) {
            this.score += 5;
            points.push((PointObject) point);
            field.remove(point);
        }

    }

    /**
     * Returns the point to the field where pacman
     * stands on
     */
    public void undoScore() {
        try {
            PointObject point = points.peek();

            if (point.getX() == field.getX() && point.getY() == field.getY()) {
                this.score -= 5;
                this.field.put(points.pop());
                this.field.notifyObservers();
            }
        } catch (EmptyStackException e) {

        }
    }

    /**
     * Returns total score of the pacman
     * 
     * @return pacman's score
     */
    public int getScore() {
        return this.score;
    }

    /**
     * Sets direction where pacman looks at
     * 
     * @param dir direction to look at
     */
    public void setDir(CommonField.Direction dir) {
        this.lookAt = dir;
    }

    /**
     * Get direction where pacman looks at
     * 
     * @return direction where pacman looks at
     */
    public CommonField.Direction getDir() {
        return this.lookAt;
    }

    /**
     * Checks whether field where pacman is contains the key.
     * If so, pacman eats it.
     * 
     * @param field field where pacman is
     */
    public void findKey(PathField field) {
        if (field.objects.isEmpty())
            return;

        CommonMazeObject key = field.objects.get(0);

        if (key instanceof KeyObject) {
            this.hasKey = true;
            this.key = (KeyObject) key;
            field.remove(key);
            field.getMaze().getMazeComponent().updateKey();
        }
    }

    /**
     * Returns the key at the field where pacman is
     */
    public void undoKey() {
        if (key == null)
            return;

        if (key.getX() == field.getX() && key.getY() == field.getY()) {
            this.field.put(key);
            this.key = null;
            field.getMaze().getMazeComponent().updateKey();
            this.field.notifyObservers();
        }
    }

    /**
     * Returns finish state of the pacman (Whether the pacman won)
     * 
     * @return true if pacman won, otherwise false
     */
    public boolean getFinished() {
        return this.isFinish;
    }

    /**
     * Returns key object if pacman has it
     * 
     * @return Returns key object
     */
    public KeyObject getKey() {
        return this.key;
    }

    /**
     * Saves logs once the pacman entered finish object
     * 
     * @param field field to check if it's finish
     */
    public void passGates(PathField field) {
        if (field.objects.isEmpty())
            return;

        this.isFinish = field.objects.get(0) instanceof FinishObject && (this.hasKey || !field.getMaze().hasKey());
        if (isFinish) {
            logger.print_logs(field.getMaze().getMazeFile().getName());
            logger.logs_clear();
        }
    }

    /**
     * Does damage to pacman
     * 
     * @param field field
     */
    public void gotHit(CommonField field) {
        if (this.lives > 1 && field.getMaze().getReview() || !field.getMaze().getReview()) {
            this.lives -= 1;
        }

        if (lives == 0) {
            logger.print_logs(field.getMaze().getMazeFile().getName());
            logger.logs_clear();
        }
    }

    /**
     * Heals pacman by 1
     */
    public void undoHit() {
        this.lives += 1;

        this.field.getMaze().getMazeComponent().updateHealth();
    }

    /**
     * Pacman meets the ghost
     * 
     * @param field field where ghost and pacman met
     */
    public void meetGhost(PathField field) {
        if (field.objects.isEmpty())
            return;

        if (field.objects.get(0) instanceof GhostObject) {
            this.gotHit(field);
        }
    }

    @Override
    public boolean move(CommonField.Direction dir, boolean isInverted) {
        if (!this.canMove(dir)) {
            return false;
        }

        Logger.log(dir, this);

        PathField nextField = (PathField) this.field.nextField(dir);

        this.field.remove(this);

        this.eatPoint(nextField);
        this.findKey(nextField);
        this.passGates(nextField);
        this.meetGhost(nextField);

        this.setField(nextField);


        this.setDir(isInverted ? this.field.getMaze().invertDirection(dir) : dir);
        
        nextField.put(this);

        this.field.getMaze().getMazeComponent().updateHealth();
        this.field.getMaze().getMazeComponent().updateScore();

        this.field.notifyObservers();

        return true;
    }

    /**
     * Returns logger that was used by pacman
     * 
     * @return logger
     */
    public Logger getLogger() {
        return logger;
    }

    @Override
    public int getLives() {
        return lives;
    }

}
