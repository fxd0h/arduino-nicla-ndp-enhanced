# Contributing to Arduino Nicla Voice NDP Library

Thank you for your interest in contributing to the Arduino Nicla Voice NDP Library! This document provides guidelines for contributing to the project.

## Getting Started

### Prerequisites
- Arduino IDE 2.0+ or Arduino CLI
- Arduino Mbed OS Nicla Boards package 4.4.1+
- Nicla Voice hardware for testing
- Git for version control

### Development Setup
1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/yourusername/arduino-nicla-ndp-enhanced.git
   cd arduino-nicla-ndp-enhanced
   ```
3. Create a new branch for your feature:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Contribution Guidelines

### Code Style
- Follow existing C++ coding conventions
- Use meaningful variable and function names
- Add comments for complex logic
- Maintain consistent indentation (4 spaces)

### Testing Requirements
- Test on actual Nicla Voice hardware
- Verify cross-platform compatibility (Linux/macOS/Windows)
- Test installation scripts on different platforms
- Ensure examples compile and run correctly

### Documentation
- Update README.md for significant changes
- Add inline comments for new functions
- Update examples if API changes
- Document any new installation requirements

## Types of Contributions

### Bug Fixes
- Identify and fix issues in the library
- Improve error handling and robustness
- Fix cross-platform compatibility issues

### Performance Improvements
- Optimize memory usage
- Improve transfer speeds
- Reduce latency in audio processing

### New Features
- Additional logging options
- Enhanced debugging capabilities
- New example applications
- Extended platform support

### Documentation
- Improve installation instructions
- Add troubleshooting guides
- Create additional examples
- Enhance technical documentation

## Pull Request Process

### Before Submitting
1. **Test thoroughly** on Nicla Voice hardware
2. **Run installation scripts** on your platform
3. **Verify examples** compile and work
4. **Update documentation** if needed
5. **Check for conflicts** with main branch

### Pull Request Template
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Performance improvement
- [ ] Documentation update

## Testing
- [ ] Tested on Nicla Voice hardware
- [ ] Verified cross-platform compatibility
- [ ] Updated examples if needed
- [ ] Installation scripts tested

## Checklist
- [ ] Code follows existing style
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No breaking changes (or documented)
```

## Development Workflow

### Branch Naming
- `feature/description` - New features
- `bugfix/description` - Bug fixes
- `docs/description` - Documentation updates
- `perf/description` - Performance improvements

### Commit Messages
Use clear, descriptive commit messages:
```
feat: add new logging control method
fix: resolve memory leak in audio processing
docs: update installation instructions
perf: optimize SPI transfer speed
```

### Testing Checklist
- [ ] Compiles without warnings
- [ ] All examples work correctly
- [ ] Installation scripts function properly
- [ ] No memory leaks or crashes
- [ ] Cross-platform compatibility maintained

## Reporting Issues

### Bug Reports
When reporting bugs, please include:
- **Platform information** (OS, Arduino IDE version)
- **Hardware details** (Nicla Voice board version)
- **Steps to reproduce** the issue
- **Expected vs actual behavior**
- **Relevant logs** or error messages
- **Screenshots** if applicable

### Feature Requests
For new features, please provide:
- **Use case description**
- **Proposed implementation**
- **Benefits to users**
- **Potential challenges**

## Code Review Process

### Review Criteria
- **Functionality** - Does it work as intended?
- **Performance** - Any impact on speed/memory?
- **Compatibility** - Works across platforms?
- **Documentation** - Is it well documented?
- **Testing** - Has it been thoroughly tested?

### Review Timeline
- Initial review within 48 hours
- Feedback and discussion as needed
- Final approval and merge

## Community Guidelines

### Communication
- Be respectful and constructive
- Provide helpful feedback
- Ask questions when unclear
- Share knowledge and experience

### Recognition
- Contributors will be credited in releases
- Significant contributions may be highlighted
- Long-term contributors may be invited as maintainers

## License

By contributing to this project, you agree that your contributions will be licensed under the same MIT License that covers the project.

## Questions?

If you have questions about contributing:
- Open a GitHub Issue for discussion
- Check existing issues for similar questions
- Review the README.md for project details
- Contact the maintainer via email: weimaraner@gmail.com

Thank you for contributing to the Arduino Nicla Voice NDP Library!
