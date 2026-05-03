# Git: Resolving Divergent Branches and Analysing History

## Problem Description
Situation: A git pull command results in a "Need to specify how to reconcile divergent branches" error.  
Cause: Another developer performed a git push --force, rewriting the history on the remote repository (origin). The local branch has an older history that no longer matches the server, preventing Git from merging them automatically.

## Commands and Parameters

### 1. Updating remote repository information
`git fetch origin`

Description: Downloads information about new commits from the server to the local .git database without altering the working directory.
Common parameters:
- --all: Downloads updates for all remote repositories.
- --prune: Removes local references to remote branches that have been deleted on the server.
- --tags: Downloads all tags from the remote repository.

### 2. Resetting to the server state (discarding local changes)
`git reset --hard origin/main`

Description: Makes the local branch identical to origin/main. All uncommitted changes and local commits not present on the server are removed from the working directory.
Used parameter:
- --hard: Resets the index and working tree.
Alternative parameters:
- --soft: Undoes local commits but leaves file changes in the index (ready to be committed).
- --mixed (default): Undoes commits and leaves file changes, but removes them from the index.
- --keep: Resets commits but attempts to preserve uncommitted local modifications.

### 3. Reapplying local commits (preserving changes)
`git rebase origin/main`

Description: Takes local commits missing from the server and applies them one by one on top of the updated history from origin/main.
Common parameters:
- -i (or --interactive): Opens an editor to squash, rename, or delete commits before applying them.
- --abort: Cancels the rebase process and restores the original state.
- --continue: Resumes the process after resolving file conflicts manually and staging them with git add.

### 4. Searching pointer history (finding lost commits)
`git reflog origin/main`

Description: Displays the local log of branch pointer movements. Useful for finding hashes of old commits before the history was rewritten.
Common parameters:
- Running without a branch name (git reflog): Shows the log for the current HEAD.
- --all: Shows the log for all local and remote branches.
- --date=relative: Displays time in a relative format (e.g., 2 hours ago).

### 5. Viewing history and generating graphs
`git log --oneline --graph --all -n 20`
`git log <new_hash>..<old_hash> --oneline`

Description: Outputs commit history. The first variation draws a tree to visualise branches. The second variation shows commits present in the old version but missing in the new one (commits overwritten by a force push).
Used parameters:
- --oneline: Condenses each commit to a single short line (hash + message).
- --graph: Draws a text-based graph on the left, showing branch divergence and merges.
- --all: Shows commits from all existing branches.
- -n 20: Limits the output to the last 20 commits.
Alternative parameters:
- --author="Name": Filters the output to show commits by a specific author.
- --stat: Shows statistics below each commit (modified files and line counts).
- --since="yesterday": Shows commits within a specific timeframe.

### 6. Viewing code differences
`git diff <new_hash> <old_hash>`

Description: Shows specific line modifications between two repository states.
Common parameters:
- --name-only: Displays only the names of modified files without the code changes.
- --cached (or --staged): Used without hashes to view changes staged via git add but not yet committed.
- --stat: Shows a brief summary of changes instead of the full code.