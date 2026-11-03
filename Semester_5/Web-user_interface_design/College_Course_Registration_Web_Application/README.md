# College Course Registration Web Application

A responsive multi-page web application for collegiate course registration and departmental exploration.

## Architecture & Pages

| Page | File | Description |
|------|------|-------------|
| **Home** | `index.html` | Hero banner, quick links, announcements, and portal navigation |
| **Courses** | `courses.html` | Comprehensive academic course catalog with credit hours, codes, and prerequisites |
| **Registration** | `registration.html` | Dynamic student registration form with interactive JavaScript validation |
| **Departments** | `departments.html` | Overview of academic branches and undergraduate/postgraduate degrees |
| **Faculty** | `faculty.html` | Profiles, designations, and research specializations of faculty members |
| **Services** | `services.html` | Academic counseling, library access, laboratory amenities, and portal support |
| **Contact** | `contact.html` | Campus location, department contact directories, and inquiry submission |

## Styling & Interaction

- `style.css` — CSS design system with responsive flex layouts, modern typography, card styling, and interactive hover states.
- `script.js` — Client-side validation: verifies registration prerequisites, email format, student roll numbers, and dynamic course selection feedback.

## Running Locally

Simply open `index.html` in any modern web browser, or launch using Python:

```bash
python -m http.server 8000
```

Then visit `http://localhost:8000`.
