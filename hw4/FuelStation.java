import java.util.Random;

public class FuelStation {
  int maxNitrogen;
  int maxQuantumFluid;
  int[] nitrogen;           // available fuel at each docking place
  int[] quantumFluid;       // available fuel at each docking place
  int dockingPlaces;        // available docking places
  int numDock;              // taken docking places
  Random r = new Random();

  public FuelStation(int maxNitrogen, int maxQuantumFluid, int dockingPlaces) {
    this.maxNitrogen = maxNitrogen;
    this.maxQuantumFluid = maxQuantumFluid;
    this.dockingPlaces = dockingPlaces;

    // initialize docking places
    nitrogen = new int[dockingPlaces];
    quantumFluid = new int[dockingPlaces];
    for (int i = 0; i < dockingPlaces; i++) {
      nitrogen[i] = maxNitrogen;
      quantumFluid[i] = maxQuantumFluid;
    }
    numDock = 0;
  }

  private synchronized int enterStation() {
    while (numDock >= dockingPlaces) {
      wait();
    }
    return numDock++;
  }

  private synchronized void leaveStation() {
    numDock--;
    if (numDock < dockingPlaces - 1) notifyAll(); // at least one docking place is free
  }

  public void getFuel(int amountN, int amountQ) {
    int dockingPlace = enterStation();  // find a dockingPlace
    while (nitrogen[dockingPlace] < amountN || quantumFluid[dockingPlace] < amountQ) {
      // give up docking place and try again
      leaveStation();
      wait();
      dockingPlace = enterStation();
    }
    nitrogen[dockingPlace] -= amountN;
    quantumFluid[dockingPlace] -= amountQ;
    Thread.sleep(r.nextInt(1000));
    leaveStation();
  }


  public void depositFuel(int amountN, int amountQ) {
    int dockingPlace = enterStation();
    if ((maxNitrogen - nitrogen[dockingPlace]) < amountN || (maxQuantumFluid - quantumFluid[dockingPlace]) < amountQ) {
      // give up docking place and try again
      leaveStation();
      wait();
      dockingPlace = enterStation();
    }
    nitrogen[dockingPlace] += amountN;
    quantumFluid[dockingPlace] += amountQ;
    Thread.sleep(r.nextInt(1000));
    notifyAll();
    leaveStation();
  }
}
