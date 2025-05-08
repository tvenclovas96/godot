import subprocess

# release build is important for benchmarks and stuff
for t in ["editor", "template_release"]:
    subprocess.run(["scons", f"target={t}"], check = True)
