use super::CommandType;

#[derive(Debug, PartialEq)]
pub enum LookupError {
    NotFound,
    NotUnique,
}

struct TrieNode {
    command_type: Option<CommandType>,
    children: Vec<(char, TrieNode)>,
}

impl TrieNode {
    fn new() -> Self {
        Self {
            command_type: None,
            children: Vec::new(),
        }
    }
}

pub struct CommandRegistry {
    root: TrieNode,
}

impl CommandRegistry {
    pub fn command_names(&self) -> Vec<String> {
        fn collect_names(node: &TrieNode, prefix: String) -> Vec<String> {
            let mut names = Vec::new();
            if node.command_type.is_some() && prefix.len() > 1 {
                names.push(prefix.clone());
            }
            for (character, node) in &node.children {
                names.append(&mut collect_names(node, format!("{}{}", prefix, character)));
            }
            names
        }

        collect_names(&self.root, "".to_owned())
    }

    pub fn lookup(&self, alias: &str) -> Result<CommandType, LookupError> {
        let mut node = &self.root;
        for character in alias.chars() {
            let maybe_existing_node = node
                .children
                .iter()
                .find(|(prefix, _)| *prefix == character);
            if let Some((_, existing_node)) = maybe_existing_node {
                node = existing_node;
            } else {
                return Err(LookupError::NotFound);
            }
        }
        if let Some(command_type) = node.command_type {
            Ok(command_type) // Exact match.
        } else {
            while node.children.len() == 1 {
                node = &node.children.first().unwrap().1;
            }
            if node.children.is_empty() && node.command_type.is_some() {
                Ok(node.command_type.unwrap()) // Partial match.
            } else {
                Err(LookupError::NotUnique)
            }
        }
    }

    pub fn new() -> Self {
        Self {
            root: TrieNode::new(),
        }
    }

    pub fn register(&mut self, alias: &str, command_type: CommandType) {
        let mut node = alias.chars().fold(&mut self.root, |node, character| {
            let children = &mut node.children;
            if let Some(i) = (0..children.len()).find(|&i| children[i].0 == character) {
                &mut children[i].1
            } else {
                children.push((character, TrieNode::new()));
                &mut children.last_mut().unwrap().1
            }
        });

        node.command_type = Some(command_type);
    }
}

#[test]
fn single_command() {
    let mut registry = CommandRegistry::new();
    registry.register("go", CommandType::Go(""));
    assert_eq!(registry.lookup("g"), Ok(CommandType::Go("")));
    assert_eq!(registry.lookup("go"), Ok(CommandType::Go("")));
    assert_eq!(registry.lookup("got"), Err(LookupError::NotFound));
}

#[test]
fn multiple_commands() {
    let mut registry = CommandRegistry::new();
    registry.register("l", CommandType::Look(""));
    registry.register("look", CommandType::Look(""));
    registry.register("lose", CommandType::Lose(""));
    assert_eq!(registry.lookup("l"), Ok(CommandType::Look("")));
    assert_eq!(registry.lookup("lo"), Err(LookupError::NotUnique));
    assert_eq!(registry.lookup("loo"), Ok(CommandType::Look("")));
    assert_eq!(registry.lookup("los"), Ok(CommandType::Lose("")));
    assert_eq!(registry.lookup("lov"), Err(LookupError::NotFound));

    // Single-character aliases are omitted from command names:
    assert_eq!(
        registry.command_names(),
        vec!["look".to_owned(), "lose".to_owned()]
    );
}
