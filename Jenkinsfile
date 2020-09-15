@Library('ctsrd-jenkins-scripts') _

class GlobalVars { // "Groovy"
    public static boolean archiveArtifacts = false;
}

// Set job properties:
def jobProperties = [rateLimitBuilds([count: 1, durationName: 'hour', userBoost: true]),
                     [$class: 'GithubProjectProperty', displayName: '', projectUrlStr: 'https://github.com/CTSRD-CHERI/FreeRTOS-mirror'],
                     copyArtifactPermission('*'), // Downstream jobs need the kernels/disk images
]
// Set the default job properties (work around properties() not being additive but replacing)
setDefaultJobProperties(jobProperties)

jobs = [:]

def runTests(params, String suffix) {
    copyArtifacts projectName: "qemu/qemu-cheri", filter: "qemu-${params.buildOS}/**", target: '.', fingerprintArtifacts: false
    sh '''
# Test running on QEMU which should exit with a status code. Timeout after 60s, blinky should exit was earlier than that
timeout 60s ./qemu-linux/bin/qemu-system-riscv64cheri -M virt -m 2048 -nographic -bios tarball/riscv64-unknown-elf/FreeRTOS/Demo/RISC-V-Generic_main_blinky.elf
'''
}

["rv64imac", "rv64imacxcheri"].each { suffix ->
    String name = "${suffix}"
    jobs[suffix] = { ->
      node("linux") {
        def PURECAP = name == "rv64imacxcheri" ? "-purecap" : ""
        def newlibRepo = gitRepoWithLocalReference(url: 'https://github.com/CTSRD-CHERI/newlib.git')       
        cheribuildProject(target: "newlib-baremetal-riscv64${PURECAP}",
                customGitCheckoutDir: 'newlib', scmOverride: newlibRepo,
                nodeLabel: null, buildStage: "Build Newlib",
                extraArgs: '--install-prefix=/',
                sdkCompilerOnly: true, skipTarball: true,
                uniqueId: "newlib-build")
          sh """
mkdir -p \$WORKSPACE/cherisdk/baremetal/baremetal-riscv64${PURECAP}
mv -f tarball/* cherisdk/baremetal/baremetal-riscv64${PURECAP}/"""

            def llvmRepo = gitRepoWithLocalReference(url: 'https://github.com/CTSRD-CHERI/llvm-project.git')
            llvmRepo["branches"] = [[name: '*/master']]
            cheribuildProject(target: "compiler-rt-builtins-baremetal-riscv64${PURECAP}",
                    customGitCheckoutDir: 'llvm-project', scmOverride: llvmRepo,
                    nodeLabel: null, buildStage: "Build compiler-rt-builtins",
                    extraArgs: '--install-prefix=/riscv64-unknown-elf',
                    sdkCompilerOnly: true, skipTarball: true,
                    uniqueId: "compiler-rt-build")
            sh """
cp -av tarball/* cherisdk/baremetal/baremetal-riscv64${PURECAP}/
# Clang searches for compiler-rt there, so copy it.
cp -av tarball/riscv64-unknown-elf/* \$(./cherisdk/bin/clang -print-resource-dir)/
"""
            cheribuildProject(target: "freertos-baremetal-riscv64${PURECAP}",
                    extraArgs: '--install-prefix=/riscv64-unknown-elf',
                    skipArchiving: true, skipTarball: true,
                    sdkCompilerOnly: true, // We only need clang not the CheriBSD sysroot since we are building FreeRTOS.
                    customGitCheckoutDir: 'freertos',
                    gitHubStatusContext: "ci/${suffix}",
                    /* Custom function to run tests since --test will not work (yet) */
                    runTests: false, afterBuild: { params -> runTests(params, suffix) })
            sh "cp -a tarball/* cherisdk/baremetal/baremetal-riscv64${PURECAP}/"
         }
    }
}

boolean runParallel = /*true*/false;
echo("Running jobs in parallel: ${runParallel}")
if (runParallel) {
    jobs.failFast = false
    parallel jobs
} else {
    jobs.each { key, value ->
        echo("RUNNING ${key}")
        value();
    }
}
