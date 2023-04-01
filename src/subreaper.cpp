#include "subreaper.h"

// References
// https://iximiuz.com/en/posts/dealing-with-processes-termination-in-Linux/
// https://stackoverflow.com/questions/56856275/is-there-some-short-example-of-how-to-use-prctl-when-it-comes-to-setting-subre

Subreaper::Subreaper(){};
Subreaper::~Subreaper(){};

// Creates a process with subreaper enabled so children get reparented to us
int Subreaper::create_process(godot::String path,
                              godot::PackedStringArray args) {
  prctl(PR_SET_CHILD_SUBREAPER, 1, 0, 0, 0);
  int pid = fork();
  if (pid < 0) {
    // Can't fork
    return -1;
  }

  // Child execution path
  if (pid == 0) {
    // Create a new session-ID so parent won't wait for it.
    // This ensures the process won't go zombie at the end.
    setsid();

    // Build the command line
    char *vargs[args.size() + 2];
    vargs[0] = strdup(path.utf8().get_data());
    int i;
    for (i = 0; i < args.size(); i++) {
      vargs[i + 1] = strdup(args[i].utf8().get_data());
    }
    vargs[i + 1] = NULL;

    execvp(path.utf8().get_data(), vargs);
    // The execvp() function only returns if an error occurs.
    ERR_PRINT("Could not create child process: " + path);
    return -1;
  }

  std::thread waitThread([]() {
    pthread_setname_np(pthread_self(), "godot-subreaper-wait");

    // Because we've set PR_SET_CHILD_SUBREAPER above, we'll get process
    // status notifications for all of our child processes, even if our
    // direct child exits. Wait until all have exited.
    while (true) {
      if (wait(nullptr) < 0) {
        if (errno == EINTR)
          continue;
        if (errno != ECHILD)
          godot::UtilityFunctions::push_error("subreaper: wait failed");
        break;
      }
    }
  });

  waitThread.detach();

  int p = 1;
  prctl(PR_GET_CHILD_SUBREAPER, &p);

  return pid;
}

// Register the methods with Godot
void Subreaper::_bind_methods() {
  // Methods
  godot::ClassDB::bind_static_method(
      "Subreaper", godot::D_METHOD("create_process", "path", "args"),
      &Subreaper::create_process);
};
