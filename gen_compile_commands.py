import json
import os
import subprocess
import shlex

def get_compile_commands_for_project(project_path):
    """Extracts compile commands from a single project directory."""
    # STM32CubeIDE usually puts the generated makefile in Debug/
    debug_dir = os.path.join(project_path, 'Debug')
    build_dir = debug_dir if os.path.exists(os.path.join(debug_dir, 'makefile')) else project_path
    
    if not os.path.exists(os.path.join(build_dir, 'makefile')) and not os.path.exists(os.path.join(build_dir, 'Makefile')):
        return []

    try:
        # Run make -Bn all to dry-run and see all commands
        result = subprocess.run(
            ['make', '-Bn', 'all'],
            cwd=build_dir,
            capture_output=True,
            text=True,
            check=True
        )
    except subprocess.CalledProcessError:
        # Some projects might not be configured yet or fail dry-run
        return []

    commands = []
    for line in result.stdout.splitlines():
        line = line.strip()
        if not line.startswith('arm-none-eabi-gcc'):
            continue
        
        args = shlex.split(line)
        if '-c' not in args:
            continue
            
        source_file = None
        for arg in args:
            if arg.endswith(('.c', '.s', '.cpp')):
                source_file = arg
                break
        
        if not source_file:
            continue
            
        abs_source_file = os.path.normpath(os.path.join(build_dir, source_file))
        
        # Normalize include paths to be absolute
        new_args = []
        for arg in args:
            if arg.startswith('-I'):
                inc_path = arg[2:]
                if inc_path.startswith('"') and inc_path.endswith('"'):
                    inc_path = inc_path[1:-1]
                abs_inc_path = os.path.normpath(os.path.join(build_dir, inc_path))
                new_args.append(f"-I{abs_inc_path}")
            elif arg == source_file:
                new_args.append(abs_source_file)
            else:
                new_args.append(arg)
        
        commands.append({
            "directory": build_dir,
            "command": shlex.join(new_args),
            "file": abs_source_file
        })
    return commands

def generate_all_compile_commands():
    workspace_root = os.getcwd()
    all_commands = []
    
    # Iterate through all subdirectories to find projects
    for root, dirs, files in os.walk(workspace_root):
        # Skip hidden directories like .git or .metadata
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        
        # STM32CubeIDE projects often have a .project file
        # We look for Makefile in root or Debug/makefile
        has_makefile = 'Makefile' in files or 'makefile' in files
        has_debug_makefile = os.path.exists(os.path.join(root, 'Debug', 'makefile'))
        
        if has_makefile or has_debug_makefile:
            print(f"Processing project: {os.path.relpath(root, workspace_root)}")
            project_commands = get_compile_commands_for_project(root)
            all_commands.extend(project_commands)
            # Avoid processing 'Debug' as a separate project if it's inside a project
            if 'Debug' in dirs:
                dirs.remove('Debug')

    output_path = os.path.join(workspace_root, 'compile_commands.json')
    with open(output_path, 'w') as f:
        json.dump(all_commands, f, indent=2)
    
    print(f"\nSuccessfully generated {output_path} with {len(all_commands)} total entries.")

if __name__ == "__main__":
    generate_all_compile_commands()
