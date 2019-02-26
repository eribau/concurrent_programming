import java.util.Random;
//import java.util.concurrent;

class FuelStation {
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
      try { wait(); }
      catch (InterruptedException e) { }
    }
    return numDock++;
  }

  private synchronized void leaveStation() {
    numDock--;
    if (numDock == 0) notifyAll(); // at least one docking place is free
  }

  public void getFuel(int amountN, int amountQ) {
    int dockingPlace = enterStation();  // find a dockingPlace
    while (nitrogen[dockingPlace] < amountN || quantumFluid[dockingPlace] < amountQ) {
      // give up docking place and try again
      leaveStation();
      try { wait(); }
      catch (InterruptedException e) { }
      dockingPlace = enterStation();
    }
    nitrogen[dockingPlace] -= amountN;
    quantumFluid[dockingPlace] -= amountQ;
    try { Thread.sleep(r.nextInt(1000)); }
    catch (InterruptedException e) { }
    leaveStation();
  }


  public void depositFuel(int amountN, int amountQ) {
    int dockingPlace = enterStation();
    if ((maxNitrogen - nitrogen[dockingPlace]) < amountN || (maxQuantumFluid - quantumFluid[dockingPlace]) < amountQ) {
      // give up docking place and try again
      leaveStation();
      try { wait(); }
      catch (InterruptedException e) { }
      dockingPlace = enterStation();
    }
    nitrogen[dockingPlace] += amountN;
    quantumFluid[dockingPlace] += amountQ;
    try { Thread.sleep(r.nextInt(1000)); }
    catch (InterruptedException e) { }
    notifyAll();
    leaveStation();
  }
}

class SpaceVehicle extends Thread {
  int id;
  int trips;
  FuelStation station;
  int maxNitrogen;
  int maxQuantumFluid;
  Random r = new Random();

  public SpaceVehicle(int id, int trips, int maxNitrogen, int maxQuantumFluid, FuelStation station) {
    this.id = id;
    this.trips = trips;
    this.station = station;
    this.maxNitrogen = maxNitrogen;
    this.maxQuantumFluid = maxQuantumFluid;
  }

  public void run() {
    int nitrogen;
    int quantumFluid;

    for (int i = 0; i <= trips; i++) {
      nitrogen = r.nextInt(maxNitrogen);
      quantumFluid = r.nextInt(maxQuantumFluid);
      System.out.println("Space vehicle " + id + " is trying to get fuel.");
      station.getFuel(nitrogen, quantumFluid);
      System.out.println("Space vehicle " + id + " has been refueled.");
      try { Thread.sleep(r.nextInt(2000)); }
      catch (InterruptedException e) { }
    }
  }
}

class SupplyVehicle extends Thread {
  int id;
  int nitrogenSupply;
  int quantumFluidSupply;
  int maxNitrogen;
  int maxQuantumFluid;
  int trips;
  FuelStation station;
  Random r = new Random();

  public SupplyVehicle(int id, int trips, int maxNitrogen, int maxQuantumFluid, FuelStation station) {
    this.id = id;
    this.trips = trips;
    this.station = station;
    this.maxNitrogen = maxNitrogen;
    this.maxQuantumFluid = maxQuantumFluid;

    nitrogenSupply = maxNitrogen * 10;
    quantumFluidSupply = maxQuantumFluid * 10;
  }

  public void run() {
    int nitrogen;
    int quantumFluid;

    for (int i = 0; i <= trips; i++) {
      nitrogen = r.nextInt(maxNitrogen);
      quantumFluid = r.nextInt(maxQuantumFluid);
      System.out.println("Supply vehicle " + id + " is trying to deposit fuel.");
      station.depositFuel(nitrogenSupply, quantumFluidSupply);
      System.out.println("Supply vehicle " + id + " succesfully deposited, is trying to get fuel.");
      station.getFuel(nitrogen, quantumFluid);
      System.out.println("Supply vehicle " + id + " has refueled.");
      try { Thread.sleep(r.nextInt(2000)); }
      catch (InterruptedException e) { }
    }
  }
}

class Main {
  public static void main(String[] arg) {
    int trips = 4;
    int dockingPlaces = 3;
    int numVeh = 5;
    int numSup = 2;
    int maxNitrogen = 10;
    int maxQuantumFluid = 10;

    if (arg.length > 0) trips = Integer.parseInt(arg[0]);
    if (arg.length > 1) dockingPlaces = Integer.parseInt(arg[1]);
    if (arg.length > 2) numVeh = Integer.parseInt(arg[2]);
    if (arg.length > 3) numSup = Integer.parseInt(arg[3]);
    if (arg.length > 4) maxNitrogen = Integer.parseInt(arg[4]);
    if (arg.length > 5) maxQuantumFluid = Integer.parseInt(arg[5]);

    System.out.println("Starting simulation");
    FuelStation station = new FuelStation(maxNitrogen, maxQuantumFluid, dockingPlaces);
    for (int id = 0; id < numVeh; id++) {
      new SpaceVehicle(id, trips, maxNitrogen, maxQuantumFluid, station).start();
    }
    for (int id = 0; id < numSup; id++) {
      new SupplyVehicle(id, trips, maxNitrogen, maxQuantumFluid, station).start();
    }
    //while(true);
    //System.out.println("Ending simulation");
  }
}
