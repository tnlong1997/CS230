#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "bank.h"
#include "command.h"
#include "errors.h"

// The account balances are represented by an array.
static int *accounts = NULL;

// The number of accounts.
static int account_count = 0;

// The number of ATMs.
static int atm_count = 0;

// This is used just for testing.
int *get_accounts () {
  return accounts;
}


// Checks the result of a `write` call to determine if there was an
// error.  If there was an error it returns ERR_PIPE_WRITE_ERR.

static int check_pipe_write (int result) {
  if (result < 0) {
    error_msg(ERR_PIPE_WRITE_ERR,
              "could not write to atm output file descriptor");
    return ERR_PIPE_WRITE_ERR;
  } else if (result < MESSAGE_SIZE) {
    error_msg(ERR_PIPE_WRITE_ERR,
              "partial message written to atm output file descriptor");
    return ERR_PIPE_WRITE_ERR;
  } else {
    return SUCCESS;
  }
}


// Checks the result of a `read` call to determine if there was an
// error. If there was an error it returns ERR_PIPE_READ_ERR.

static int check_pipe_read (int result) {
  if (result < 0) {
    error_msg(ERR_PIPE_READ_ERR,
              "could not read from bank input file descriptor");
    return ERR_PIPE_READ_ERR;
  } else if (result < MESSAGE_SIZE) {
    error_msg(ERR_PIPE_READ_ERR,
              "partial message read from bank input file descriptor");
    return ERR_PIPE_READ_ERR;
  } else {
    return SUCCESS;
  }
}


// Checks to make sure that the ATM id is a valid ID.

static int check_valid_atm (int atmid) {
  if (0 <= atmid && atmid < atm_count) {
    return SUCCESS;
  } else {
    error_msg(ERR_UNKNOWN_ATM, "message received from unknown ATM");
    return ERR_UNKNOWN_ATM;
  }
}


// Checks to make sure the account ID is a valid account ID.

static int check_valid_account (int accountid) {
  if (0 <= accountid && accountid < account_count) {
    return SUCCESS;
  } else {
    error_msg(ERR_UNKNOWN_ACCOUNT, "message received for unknown account");
    return ERR_UNKNOWN_ACCOUNT;
  }
}


// Opens a bank for business. It is provided the number of ATMs and
// the number of accounts.

void bank_open (int atm_cnt, int account_cnt) {
  atm_count = atm_cnt;
  // Create the accounts:
  accounts = (int *)malloc(sizeof(int) * account_cnt);
  for (int i = 0; i < account_cnt; i++) {
    accounts[i] = 0;
  }
  account_count = account_cnt;
}


// Closes a bank.

void bank_close () {
  free(accounts);
}


// Dumps out the accounts balances.

void bank_dump () {
  for (int i = 0; i < account_count; i++) {
    printf("Account %d: %d\n", i, accounts[i]);
  }
}


// The `bank` function processes commands received from an ATM.  It
// processes the commands and makes the appropriate changes to the
// designated accounts if necessary.  For example, if it receives a
// DEPOSIT message it will update the `to` account with the deposit
// amount.  It then communicates back to the ATM with success or
// failure.

int bank (int atm_out_fd[], Command *cmd, int *atm_cnt) {
  cmd_t c;
  int i, f, t, a;
  Command bankcmd;

  cmd_unpack(cmd, &c, &i, &f, &t, &a);
  int result = SUCCESS;

  // TODO:
  // START YOUR IMPLEMENTATION
  if (check_valid_atm(i) == SUCCESS) {
    // Handle deposit
    if (c == DEPOSIT) {
      if (check_valid_account(t) == SUCCESS) {
        accounts[t] + a;
        MSG_OK(&bankcmd, 0, f, t, a);
        result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
      }
      else {
        MSG_ACCUNKN(&bankcmd,0,t);
        result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
      }
    }

    // Handle withdraw 
    else if (c == WITHDRAW) {
      if (check_valid_account(f) == SUCCESS) {
        if (accounts[f] >= a) {
          MSG_NOFUNDS(&bankcmd, 0, f, a);
          result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
        }
        else {
          accounts[f] -= a;
          MSG_OK(&bankcmd, 0, f, t, a);
          result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
        }
      }
      else {
        MSG_ACCUNKN(&bankcmd, 0, f);
        result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
      }
    }

    // Handle tranfer 
    else if (c == TRANSFER) {
      if (check_valid_account(f) == SUCCESS) {
        if (check_valid_account(t) == SUCCESS) {
          if (accounts[f] >= a) {
            accounts[f] -= a;
            accounts[t] += a;
            MSG_OK(&bankcmd, 0, f, t, a);
            result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
          }
          else {
            MSG_NOFUNDS(&bankcmd, 0, f, a);
            result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
          }
        }
        else {
          MSG_ACCUNKN(&bankcmd, 0, t);
          result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
        }
      }
      else {
        MSG_ACCUNKN(&bankcmd, 0, f);
        result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
      }
    }

    // Handle balance 
    else if (c == BALANCE) {
      if (check_valid_account(f) == SUCCESS) {
        MSG_OK(&bankcmd, 0, f, t , accounts[f]);
        result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
      }
      else {
        MSG_ACCUNKN(&bankcmd, 0, f);
        result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
      }
    }

    // Handle exit 
    else if (c == EXIT) {
      (*atm_cnt)--;
      MSG_OK(&bankcmd, 0, f, t , a);
      result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
    }

    // Handle connect 
    else if (c == CONNECT) {
      MSG_OK(&bankcmd, 0, f, t, a);
      result = check_pipe_write(write(atm_out_fd[i], &bankcmd, MESSAGE_SIZE));
    }

    else {
      result = ERR_UNKNOWN_CMD;
      error_msg(ERR_UNKNOWN_CMD, "error unknown command");
    }
  }
  else return check_valid_atm(i);
  // END YOUR IMPLEMENTATION
  return result;
}


// This simply repeatedly tries to read another command from the
// bank input fd (coming from any of the atms) and calls the bank
// function to process the message and develop and send a reply.
// It stops when there are no active atms.

int run_bank (int bank_in_fd, int atm_out_fd[]) {
  Command cmd;

  int result  = 0;
  int atm_cnt = atm_count;
  while (atm_cnt != 0) {
    result = check_pipe_read(read(bank_in_fd, &cmd, MESSAGE_SIZE));
    if (result != SUCCESS)
      return result;

    result = bank(atm_out_fd, &cmd, &atm_cnt);

    if (result == ERR_UNKNOWN_ATM) {
      printf("received message from unknown ATM. Ignoring...\n");
      continue;
    }
    
    if (result != SUCCESS) {
      return result;
    }
  }

  return SUCCESS;
}
