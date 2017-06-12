# --------------------------------------------------------------------- #
# This is an script originally written by Octonezd.
# This script has been editted to make it work for TWLoader by Jolty95
# --------------------------------------------------------------------- #
try:
    from base64 import b64encode
    from os import getenv
    from json import dumps
    from subprocess import check_output
    import requests
    itemstopush = {
        "Nightly/TWLoader.zip": "TWLoader.zip",
        "Nightly/TWLoader.cia": "TWLoader-beta.cia"
    }
    version = str(
        check_output('git log -n 1 --pretty=format:"%h"', shell=True), 'utf-8')
    github = "https://api.github.com/repos/Jolty95/TWLoader-update/"
    branch = "master"
    folder = "beta"
    treesha = ""

    def upload(file, filename):
        fileurl = github + "contents/" + folder + "/%s"
        print("Uploading", file, "as", filename, end=" ")
        reqjs = {"message": "File:%s Commit:%s" % (filename, version),
                 "author": {
                 "name": "Jolty95",
                 "email": "Jolty95@users.noreply.github.com"
        },
            "branch": branch,
        }
        with open(file, 'rb') as f:
            reqjs["content"] = str(b64encode(f.read()), 'utf-8')
        tree = requests.get(github + "git/trees/heads/" + branch).json()
        for file in tree["tree"]:
            if file["path"] == folder:
                treesha = file["sha"]
                break
        tree2 = requests.get(github + "git/trees/" + treesha).json()
        for file in tree2["tree"]:
            if file["path"] == filename:
                reqjs["sha"] = file["sha"]
                break
        req = dumps(reqjs)
        headers = {"Authorization": "token " + getenv("GITHUBAUTH")}
        r = requests.put(fileurl % (filename), headers=headers, data=req)
        if r.status_code == 201 or r.status_code == 200:
            print("[OK]")
        else:
            print("[FAIL:", r.status_code, "]", sep="")

    for file in itemstopush:
        upload(file, itemstopush[file])
except Exception as e:
    print("An error occured during uploading nightly build.")
    print(e)
raise SystemExit
