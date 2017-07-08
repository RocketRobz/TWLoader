# --------------------------------------------------------------------- #
# This script has been created for TWLoader by Jolty95
# --------------------------------------------------------------------- #
import json
import requests
import git
from shutil import copyfile
print("Patching Makefile...")
with open("gui/Makefile", "r") as f:
    makefile = f.read().replace("IS_NIGHTLY := 0", "IS_NIGHTLY := 1")
with open("gui/Makefile", "w") as f:
    f.write(makefile)
    f.close()
print("Patching ROMFS...")
copyfile("romfs/logo.png", ".gui/data/logo.png")
copyfile("romfs/logo_demo.png", "gui/data/logo_demo.png")
print("Patching Nightly hash...")
commit_json = requests.get(
    "https://raw.githubusercontent.com/Jolty95/TWLoader-update/master/beta/updatenightlies.json").json()
with open("updatenightlies.json", "w") as updatenightlies:
    repo = git.Repo(search_parent_directories=True)
    sha = repo.head.object.hexsha
    commit_json["nightlies"]["commit"] = sha
    updatenightlies.write(json.dumps(commit_json, sort_keys=True, indent=4, separators=(',', ': ')))
print("All done!")
