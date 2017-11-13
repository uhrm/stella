//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2017 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef DEBUGGER_PARSER_HXX
#define DEBUGGER_PARSER_HXX

#include <functional>
#include <sstream>
#include <set>

class Debugger;
class FilesystemNode;
struct Command;

#include "bspf.hxx"
#include "FrameBuffer.hxx"
#include "Settings.hxx"

class DebuggerParser
{
  public:
    DebuggerParser(Debugger& debugger, Settings& settings);

    /** Run the given command, and return the result */
    string run(const string& command);

    /** Execute parser commands given in 'file' */
    string exec(const FilesystemNode& file, StringList* history = nullptr);

    /** Given a substring, determine matching substrings from the list
        of available commands.  Used in the debugger prompt for tab-completion */
    void getCompletions(const char* in, StringList& list) const;

    /** Evaluate the given expression using operators, current base, etc */
    int decipher_arg(const string& str);

    /** String representation of all watches currently defined */
    string showWatches();

    static inline string red(const string& msg = "")
    {
      return char(kDbgChangedColor) + msg;
    }
    static inline string inverse(const string& msg = "")
    {
      // ASCII DEL char, decimal 127
      return "\177" + msg;
    }

  private:
    bool getArgs(const string& command, string& verb);
    bool validateArgs(int cmd);
    string eval();
    string saveScriptFile(string file);

  private:
    enum { kNumCommands = 87 };

    // Constants for argument processing
    enum {
      kIN_COMMAND,
      kIN_SPACE,
      kIN_BRACE,
      kIN_ARG
    };

    enum parameters {
      kARG_WORD,        // single 16-bit value
      kARG_DWORD,       // single 32-bit value
      kARG_MULTI_WORD,  // multiple 16-bit values (must occur last)
      kARG_BYTE,        // single 8-bit value
      kARG_MULTI_BYTE,  // multiple 8-bit values (must occur last)
      kARG_BOOL,        // 0 or 1 only
      kARG_LABEL,       // label (need not be defined, treated as string)
      kARG_FILE,        // filename
      kARG_BASE_SPCL,   // base specifier: 2, 10, or 16 (or "bin" "dec" "hex")
      kARG_END_ARGS     // sentinel, occurs at end of list
    };

    struct Command {
      string cmdString;
      string description;
      string extendedDesc;
      bool parmsRequired;
      bool refreshRequired;
      parameters parms[10];
      std::function<void (DebuggerParser*)> executor;
    };
    struct Trap
    {
      bool read;
      bool write;
      uInt32 begin;
      uInt32 end;
      string condition;
    };

    // Reference to our debugger object
    Debugger& debugger;

    // Reference to settings object (required for saving certain options)
    Settings& settings;

    // The results of the currently running command
    ostringstream commandResult;

    // currently execute command id
    int myCommand;
    // Arguments in 'int' and 'string' format for the currently running command
    IntArray args;
    StringList argStrings;
    uInt32 argCount;

    StringList myWatches;

    // Keep track of traps (read and/or write)
    vector<unique_ptr<Trap>> myTraps;
    void listTraps(bool listCond);
    string trapStatus(const Trap& trap);

    // output the error with the example provided for the command
    void outputCommandError(const string& errorMsg, int command);

    // List of available command methods
    void executeA();
    void executeBase();
    void executeBreak();
    void executeBreakif();
    void executeC();
    void executeCheat();
    void executeClearbreaks();
    void executeClearconfig();
    void executeCleartraps();
    void executeClearwatches();
    void executeCls();
    void executeCode();
    void executeColortest();
    void executeD();
    void executeData();
    void executeDebugColors();
    void executeDefine();
    void executeDelbreakif();
    void executeDelfunction();
    void executeDeltrap();
    void executeDelwatch();
    void executeDisasm();
    void executeDump();
    void executeExec();
    void executeExitRom();
    void executeFrame();
    void executeFunction();
    void executeGfx();
    void executeHelp();
    void executeJoy0Up();
    void executeJoy0Down();
    void executeJoy0Left();
    void executeJoy0Right();
    void executeJoy0Fire();
    void executeJoy1Up();
    void executeJoy1Down();
    void executeJoy1Left();
    void executeJoy1Right();
    void executeJoy1Fire();
    void executeJump();
    void executeListbreaks();
    void executeListconfig();
    void executeListfunctions();
    void executeListtraps();
    void executeLoadconfig();
    void executeLoadstate();
    void executeN();
    void executePalette();
    void executePc();
    void executePGfx();
    void executePrint();
    void executeRam();
    void executeReset();
    void executeRewind();
    void executeRiot();
    void executeRom();
    void executeRow();
    void executeRun();
    void executeRunTo();
    void executeRunToPc();
    void executeS();
    void executeSave();
    void executeSaveconfig();
    void executeSavedisassembly();
    void executeSaverom();
    void executeSaveses();
    void executeSavesnap();
    void executeSavestate();
    void executeScanline();
    void executeStep();
    void executeTia();
    void executeTrace();
    void executeTrap();
    void executeTrapif();
    void executeTrapread();
    void executeTrapreadif();
    void executeTrapwrite();
    void executeTrapwriteif();
    void executeTraps(bool read, bool write, const string& command, bool cond = false);
    void executeTrapRW(uInt32 addr, bool read, bool write, bool add = true);  // not exposed by debugger
    void executeType();
    void executeUHex();
    void executeUndef();
    void executeUnwind();
    void executeV();
    void executeWatch();
    void executeX();
    void executeY();
    void executeZ();

    // List of commands available
    static Command commands[kNumCommands];

  private:
    // Following constructors and assignment operators not supported
    DebuggerParser() = delete;
    DebuggerParser(const DebuggerParser&) = delete;
    DebuggerParser(DebuggerParser&&) = delete;
    DebuggerParser& operator=(const DebuggerParser&) = delete;
    DebuggerParser& operator=(DebuggerParser&&) = delete;
};

#endif
