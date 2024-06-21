package game.common;

import java.util.HashSet;
import java.util.Set;

/**
 * An abstract class implementing the operations of the Observable interface. It
 * allows inserting and canceling observers and notifying registered observers
 * about changes.
 * 
 * @author Oleksandr Turytsia (xturyt00)
 * @author Kambulat Alakaev (xalaka00)
 * @version 1.0
 */
public abstract class AbstractObservableField implements CommonField {
    private final Set<Observable.Observer> observers = new HashSet<>();

    @Override
    public void addObserver(Observable.Observer o) {
        this.observers.add(o);
    }

    @Override
    public void removeObserver(Observable.Observer o) {
        this.observers.remove(o);
    }

    @Override
    public void notifyObservers() {
        this.observers.forEach((o) -> {
            o.update(this);
        });
    }
}
