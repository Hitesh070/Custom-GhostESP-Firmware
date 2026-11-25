"""GhostESP Command Center — Engineering Dark Theme & Design Tokens."""

PALETTE = {
    "bg_canvas": "#090d14",
    "bg_sidebar": "#0d131d",
    "bg_surface": "#111824",
    "bg_elevated": "#172130",
    "bg_input": "#0c111a",
    "bg_console": "#06090f",
    "border_subtle": "#1e293b",
    "border_active": "#00e5a0",
    "text_primary": "#e2e8f0",
    "text_secondary": "#94a3b8",
    "text_muted": "#64748b",
    "accent_green": "#00e5a0",
    "accent_cyan": "#38bdf8",
    "accent_amber": "#f59e0b",
    "accent_red": "#f43f5e",
    "accent_purple": "#a855f7",
}

APP_STYLESHEET = f"""
QMainWindow, QWidget {{
    background-color: {PALETTE['bg_canvas']};
    color: {PALETTE['text_primary']};
    font-family: "Segoe UI", "Inter", sans-serif;
    font-size: 12px;
}}

QFrame#Sidebar {{
    background-color: {PALETTE['bg_sidebar']};
    border-right: 1px solid {PALETTE['border_subtle']};
}}

QFrame#TopBar {{
    background-color: {PALETTE['bg_surface']};
    border-bottom: 1px solid {PALETTE['border_subtle']};
}}

QFrame#Card {{
    background-color: {PALETTE['bg_surface']};
    border: 1px solid {PALETTE['border_subtle']};
    border-radius: 6px;
}}

QLabel#SectionTitle {{
    color: {PALETTE['accent_cyan']};
    font-weight: 700;
    font-size: 11px;
    letter-spacing: 0.8px;
}}

QLabel#MetricValue {{
    color: {PALETTE['text_primary']};
    font-family: "Consolas", "Cascadia Mono", monospace;
    font-size: 20px;
    font-weight: 700;
}}

QLabel#BadgeSimulated {{
    background-color: #2c1e09;
    color: {PALETTE['accent_amber']};
    border: 1px solid #78480a;
    border-radius: 4px;
    padding: 2px 6px;
    font-weight: 700;
    font-size: 10px;
}}

QLabel#BadgeLive {{
    background-color: #06291e;
    color: {PALETTE['accent_green']};
    border: 1px solid #0e5a42;
    border-radius: 4px;
    padding: 2px 6px;
    font-weight: 700;
    font-size: 10px;
}}

QLabel#BadgeOffline {{
    background-color: #2b0b14;
    color: {PALETTE['accent_red']};
    border: 1px solid #6b1a2f;
    border-radius: 4px;
    padding: 2px 6px;
    font-weight: 700;
    font-size: 10px;
}}

QPushButton {{
    background-color: {PALETTE['bg_elevated']};
    color: {PALETTE['text_primary']};
    border: 1px solid {PALETTE['border_subtle']};
    border-radius: 4px;
    padding: 5px 11px;
    font-weight: 600;
}}

QPushButton:hover {{
    border-color: {PALETTE['accent_cyan']};
    background-color: #1e2a3d;
}}

QPushButton:pressed {{
    background-color: #25344b;
}}

QPushButton:disabled {{
    color: {PALETTE['text_muted']};
    background-color: {PALETTE['bg_input']};
}}

QPushButton#PrimaryBtn {{
    background-color: {PALETTE['accent_green']};
    color: #04120c;
    border: 1px solid {PALETTE['accent_green']};
}}

QPushButton#PrimaryBtn:hover {{
    background-color: #22f2b4;
}}

QPushButton#DangerBtn {{
    background-color: {PALETTE['accent_red']};
    color: #ffffff;
    border: 1px solid {PALETTE['accent_red']};
}}

QPushButton#NavBtn {{
    text-align: left;
    padding: 7px 12px;
    border: none;
    border-left: 3px solid transparent;
    border-radius: 0px;
    background-color: transparent;
    color: {PALETTE['text_secondary']};
    font-weight: 600;
}}

QPushButton#NavBtn:hover {{
    background-color: {PALETTE['bg_elevated']};
    color: {PALETTE['text_primary']};
}}

QPushButton#NavBtn[active="true"] {{
    background-color: {PALETTE['bg_elevated']};
    color: {PALETTE['accent_green']};
    border-left: 3px solid {PALETTE['accent_green']};
}}

QLineEdit, QComboBox, QSpinBox {{
    background-color: {PALETTE['bg_input']};
    color: {PALETTE['text_primary']};
    border: 1px solid {PALETTE['border_subtle']};
    border-radius: 4px;
    padding: 4px 8px;
    font-family: "Consolas", "Cascadia Mono", monospace;
}}

QLineEdit:focus, QComboBox:focus, QSpinBox:focus {{
    border-color: {PALETTE['accent_cyan']};
}}

QPlainTextEdit, QTextEdit {{
    background-color: {PALETTE['bg_console']};
    color: {PALETTE['text_primary']};
    border: 1px solid {PALETTE['border_subtle']};
    border-radius: 4px;
    font-family: "Consolas", "Cascadia Mono", monospace;
    font-size: 11px;
}}

QTableWidget, QTreeWidget {{
    background-color: {PALETTE['bg_surface']};
    alternate-background-color: #0e1520;
    color: {PALETTE['text_primary']};
    gridline-color: {PALETTE['border_subtle']};
    border: 1px solid {PALETTE['border_subtle']};
    border-radius: 4px;
    selection-background-color: #193247;
    selection-color: #ffffff;
}}

QHeaderView::section {{
    background-color: {PALETTE['bg_elevated']};
    color: {PALETTE['accent_cyan']};
    border: none;
    border-bottom: 1px solid {PALETTE['border_subtle']};
    border-right: 1px solid {PALETTE['border_subtle']};
    padding: 5px 8px;
    font-weight: 700;
    font-size: 11px;
}}

QStatusBar {{
    background-color: {PALETTE['bg_sidebar']};
    color: {PALETTE['text_secondary']};
    border-top: 1px solid {PALETTE['border_subtle']};
}}
"""
