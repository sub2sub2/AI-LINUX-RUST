// Define an enum for different roles
#[derive(Debug, PartialEq, Clone, Copy)]
pub enum Role {
    Admin,
    Moderator,
    User,
}


#[derive(Debug, Clone)]
pub struct AppBase {
    appname: String,
    address: String,
    role: Role,
}

impl AppBase {
    pub fn new (name: String, addr:String, role: Role) -> Self {
        Self {
            appname: name,
            address: addr,
            role: role
        }
    }

    // Check if the user has permission to perform an action
    pub fn has_permission(&self, required_role: Role) -> bool {
        // Simple authorization logic based on user's role
        dbg!("app {:?} has_permission?  
        app role:{:?} == required_role: {:?}? ", self.appname(), self.role(), required_role);
        match (self.role, required_role) {
            (Role::Admin, _) => true, // Admin has all permissions
            (Role::Moderator, Role::Moderator) => true, // Moderator has permission to moderate
            (Role::User, Role::User) => true, // User has basic permissions
            (_, _) => false, // Any other combination does not have permission
        }
    }
    pub fn appname(&self) -> &str {
        &self.appname
    }

    pub fn address(&self) -> &str {
        &self.address
    }
    
    pub fn role(&self) -> Role {
        self.role
    }
    
    pub fn set_role(&mut self, role: Role) {
        self.role = role;
    }
    
    
}
