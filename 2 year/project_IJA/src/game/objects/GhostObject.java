package game.objects;

import game.common.CommonField;
import game.common.CommonField.Direction;

import java.util.Arrays;
import java.util.List;
import java.util.Random;

import common.Logger;

/**
 * Class representing ghost object
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public class GhostObject extends MazeObject {

    /**
     * Creates ghost object in a given field
     * 
     * @param field given field
     */
    public GhostObject(CommonField field) {
        super(field);
    }

    @Override
    public boolean canMove(CommonField.Direction dir) {
        return field.nextField(dir) instanceof PathField;
    }

    /**
     * Checks if the field, where ghost is, contains
     * pacman. If so, does damage to him.
     * 
     * @param field field that needs to be checked for pacman
     */
    public void meetPacman(PathField field) {

        PacmanObject pacman = field.getMaze().getPacman();

        if (!field.objects.contains(pacman))
            return;

        pacman.gotHit(field);
    }

    /**
     * Ghost movement implementation
     */
    public void startMoving() {
        List<CommonField.Direction> directions = Arrays.asList(CommonField.Direction.R, CommonField.Direction.L,
                CommonField.Direction.U, CommonField.Direction.D);

        Random random = new Random();
        PacmanObject pacman = this.field.getMaze().getPacman();

        try {
            while (true) {
                CommonField.Direction dir = directions.get(random.nextInt(directions.size()));

                if (pacman.getFinished() || pacman.getLives() <= 0 || field.getMaze().getPause())
                    break;

                if (!this.canMove(dir))
                    continue;

                this.move(dir, false);

                Thread.sleep(250);
            }
        } catch (InterruptedException e) {

        }

    }

    @Override
    public boolean move(Direction dir, boolean isInverted) {
        if (!this.canMove(dir)) {
            return false;
        }

        Logger.log(dir, this);

        PathField nextField = (PathField) this.field.nextField(dir);

        this.field.remove(this);

        this.meetPacman(nextField);

        this.setField(nextField);

        nextField.put(this);

        this.field.getMaze().getMazeComponent().updateHealth();

        this.field.notifyObservers();

        return true;
    }

    @Override
    public int getLives() {

        throw new UnsupportedOperationException("Unimplemented method 'getLives'");
    }

    @Override
    public boolean isPacman() {
        return false;
    }
}
